#include "CalibrationAxe.h"
//*****************************************************************************//
//                                     PUBLIC
//*****************************************************************************//
/**
 * @brief Permet d'initialiser tout les parametre de la class CalibrationAxe
 * 
 * @param pinAnalog : Pin Analog qui est connecter a l'accelerometre 
 * @param zero  : La valeur lu par l'accelerometre au repos ( 0 G) 
 * @param span  : La valeur pour convertir le RAW en m/s2 
 * @param pinDigital  : Pin Digital qui permet d'activer l'axe 
 * @param nomAxe    : Le nom de l'axe afin de reconnaitre lors de calibration (PORT SERIE)
 * @param pinDigitalDisplayClk : Pin Digital CLK d'un afficher 7 segment
 * @param pinDigitalDisplayData  : Pin Digital Data d'un afficheur 7 segment
 * @param pinAnalogOffset   : Pin Analog qui permet de contreler le offset
 */
 void CalibrationAxe::init(uint8_t pinAnalog, uint16_t zero, float span, 
                            uint8_t pinDigital, String nomAxe, uint8_t pinDigitalDisplayPositionClk, 
                            uint8_t pinDigitalDisplayPositionData,uint8_t pinDigitalDisplayPoidsClk,uint8_t pinDigitalDisplayPoidsData, uint8_t pinAnalogOffset,float constanteDeRapel, int16_t offsetCoded)
    {
        Acc.init(pinAnalog, zero, span);
        NomAxe = nomAxe;
        pinActiveAxe = pinDigital;
        pinMode(pinActiveAxe, INPUT_PULLUP);
        resetData();
        displayPosition.init(pinDigitalDisplayPositionClk, pinDigitalDisplayPositionData);
        displayPosition.set(LUMINOSITE);
        //displayPosition.point(POINT_ON);
        displayPoids.init(pinDigitalDisplayPoidsClk,pinDigitalDisplayPoidsData);
        displayPoids.set(LUMINOSITE);
        //displayPoids.point(POINT_ON);
        pinOffset = pinAnalogOffset;
        blinkEtat = 0;
        erreur = 0;
        AngleAccMinOffset_0_100 = 0;
        AngleAccMaxOffset_0_100 = 0;
        ConstanteDeRapel_K = constanteDeRapel;
        OffsetCoded = offsetCoded;
    }


    /**
     * @brief Permet de faire le test de l'axe 
     *        Avec l'interuption dans le Main
     *        Le module se s'incronise afin de prendre les mesure de test au bon moment.
     * 
     * Apres se test : 
     *         L'obtention de la position et de la masse debalancer et son poids
     *         L'affichage sur les 7 segment de la position et du poids
     * @param rpm : la valeur du rpm qui permet de calculer les temps entre les prises de valeur
     */
    void CalibrationAxe::test(uint16_t rpm, float diametre)
    {
        // reset 
        AngleAccMinOffset_0_100 = 0;
        AngleAccMaxOffset_0_100 = 0;
        erreur = 0;
        blinkEtat = 0;

        if (checkAxeActive())
        {

            int i = 0;
            int j = 0;
            resetData();
            uint32_t timeEntreLecture = ((60.0 / (double)rpm) / (double)NB_LECTURE) * (double)1000000; // (Periode de rotation / NB_LECTURE) 1 000 000 micros seconde
            uint32_t timerOlder = micros();
            FlagNewRotation = 0;
            while (i < NB_TEST)
            {
                if (FlagNewRotation == 1) // Etre certain que on vien de detecter un rotation
                {
                    while (j < NB_LECTURE && FlagNewRotation == 1) // tant qu"on a pas fait le tour ou que la rotation est fini
                    {
                        if (micros() - timerOlder > timeEntreLecture)
                        {
                            timerOlder = micros();
                            RawAcc[j] += Acc.read();
                            j++;
                        }
                    }
                    while (j < NB_LECTURE)
                    {
                        RawAcc[j] += Acc.Zero; // Finaliser les donnee avec une valeur sans impact ZERO de l'acc
                        j++;
                    }
                    NbLectureTotal += j;
                    j = 0;
                    i++;
                    FlagNewRotation = 0;
                }
                else
                {
                    FlagNewRotation = 0;
                }
            }
            for (i = 0; i < NB_LECTURE; i++)
            {
                RawAcc[i] = RawAcc[i] / NB_TEST; //Recontruire la moyenne des points
            }
            
            
            algoPeak();     
            convertirPoidCalcul(rpm);
            convertirPoidEnGrammeCalcul( rpm, diametre);
            prepareDataAfficher(); 
            displayValue(); 

            #if _DEBUG_N1_
            afficherInfoTest();
            #endif
            
        }
    }

    /**
     * @brief 
     * 
     */
    void CalibrationAxe::changeScaleData_0_360()
    {
        AngleAccMaxOffset_0_360 = AngleAccMaxOffset_0_100 * 3.6;
        AngleAccMinOffset_0_360 = AngleAccMinOffset_0_100 * 3.6;
    } 

    /**
     * @brief Permet afficher la moyenne de toute les prises d'une rotation du shaft
     * 
     * @param info : String qui permet d'informer du type d'infomation suivant
     */
    void CalibrationAxe::afficherTest(String info) // affiche le data 
    {
        Serial.println(info);
        Serial.print("NOMBRE de TEST MOYENNE : ");
        Serial.println(NbLectureTotal);
        for (int i = 0; i < NB_LECTURE; i++)
        {
            Serial.print(i);
            Serial.print(";");
            Serial.println(RawAcc[i]);
        }
    }

    
    
    /**
     * @brief Permet d'afficher sur le port seriel les information pertinante a la calibration
     * 
     */
    void CalibrationAxe::afficherPeak()
    {
        Serial.println("\n-------------------------------------");
        Serial.print("NOM AXE : ");Serial.println(NomAxe);
        Serial.println("-------------------------------------");
        Serial.print("Peak Min : ");Serial.println(AccMin_raw);
        Serial.print("Angle Min 0 - 99  : ");Serial.println(AngleAccMin_0_100);
        Serial.print("Peak Max : ");Serial.println(AccMax_raw);
        Serial.print("Angle Max 0 - 99  : ");Serial.println(AngleAccMax_0_100);
        Serial.print("Valeur avec offset min :");Serial.println(AngleAccMinOffset_0_100);
        Serial.print("Valeur avec offset Max :");Serial.println(AngleAccMaxOffset_0_100);
        Serial.print("POIDS CALCULER : "); Serial.println(poidCalculer);
        Serial.println("==================================");
        Serial.print("Afficher m/s2 force Min : ");Serial.println(Acc.convertRawToGForce(AccMin_raw));
        Serial.print("Afficher m/s2 force Max : ");Serial.println(Acc.convertRawToGForce(AccMax_raw));
        Serial.print("Afficher m/s2 force Diff : ");Serial.println(Acc.convertRawToGForce(AccMax_raw) - Acc.convertRawToGForce(AccMin_raw) );
        Serial.print("Masse unbalance(g) : ");Serial.println(MasseUnbalance_g);
        Serial.print("Masse Unbalance (oz) : ");Serial.println(MasseUnbalance_oz);
        Serial.print("Masse unbalance 2(g) : ");Serial.println(MasseUnbalance_g2);
        Serial.print("Masse Unbalance 2(oz) : ");Serial.println(MasseUnbalance_oz2);
        Serial.println("==================================");
        Serial.println("-------------------------------------");
    }

    /**
     * @brief Permet d'afficher les informations pertinante lier au poids sur le port serial
     * 
     */
    void CalibrationAxe::afficherInfoCalibPoids()
    {
        Serial.println(AccMin_raw);
        Serial.println(AccMax_raw);
        Serial.println(abs(AccMax_raw-AccMin_raw));
        Serial.println(Acc.convertRawToGForce(AccMin_raw));
        Serial.println(Acc.convertRawToGForce(AccMax_raw));
        Serial.println(Acc.convertRawToGForce(AccMax_raw)-Acc.convertRawToGForce(AccMin_raw));
    }


    /**
     * @brief Permet de determiner si il y a eu une erreur potentiel lors du test 
     *          Lorsque la difference entre le peak min et max difere de 180 +- 15 
     *          un flag est lever afin d'en informer l'operateur
     * 
     * @return true : Lors qu il y a une erreur superieur 
     * @return false : correcte
     */
    bool CalibrationAxe::checkErreur()
    {   //(A MODIFIER)
        uint16_t tmp = abs(AngleAccMaxOffset_0_100-AngleAccMinOffset_0_100); // devrai pt refgarder cette information avec un autre variable qui est fixe avant la conversion 
        if( tmp > 15 && tmp < 75) // Valeur agle diff inacceptable                // FAUX car c'est avec le offset !!!! 
        {
            erreur = TRUE;
        }
        else
        {
            erreur = FALSE;
        }

        return erreur;   
    }




    /**
     * @brief Permet de faire blinker l'affichage 
     *  (Lors d'une erreur par example) Communiquer d'avantage d'info
     * 
     */
    void CalibrationAxe::blinkDisplay()
    {
        if(blinkEtat == 0)
        {
            displayPosition.clearDisplay();
            displayPoids.clearDisplay();
            blinkEtat = 1;
        }
        else
        {
            displayValue();
            blinkEtat = 0;
        }
        
    }
    /**
     * @brief Permet d'editer le offset de l'axe
     * 
     */
    void CalibrationAxe::editOffset()
    {
        int16_t offset;
        uint8_t buff[4];
        offset = map(analogRead(pinOffset), 0, 1023, -50, 50);
        Serial.println(offset);
        split_4_digit_number(abs(Offset), buff);

        displayPosition.display(1, buff[1]);
        displayPosition.display(2, buff[2]);
        displayPosition.display(3, buff[3]);
        if (Offset < 0)
        {
            displayPosition.displaySeg(0, 0b01000000);
        }
        else
        {
            displayPosition.displaySeg(0, 0b00000000);
        }

        if (offset != Offset)
        {
            Offset = offset; 
        }
    }

    /**
     * @brief Permet de faire la calibration de l'axe et de l'afficher sur le moniteur serial
     * 
     */
    void CalibrationAxe::calibrationZeroAcc()
    {
        Acc.calibrationZero();
        afficherCalibZero(); 
    }

    /**
     * @brief Permet d'afficher le zero de l'accelerometre
     * 
     */
    void CalibrationAxe::afficherCalibZero()
    {
        Serial.println("=================Calibration Zero=======================");
        Serial.println(NomAxe);
        Serial.print("Zero : ");Serial.println(Acc.Zero);
        Serial.println("========================================================");
    }

    /**
     * @brief Permet d'afficher les information suite a un test sur le port serial
     * 
     */
    void CalibrationAxe::afficherInfoTest()
    {
            //afficherTest("Test Apres Moyenne");
            afficherPeak();
           // afficherInfoCalibPoids();

            
    }



//*****************************************************************************//
//                                     PRIVATE
//*****************************************************************************//

    /**
     * @brief Permet de reset le tableau des valeurs de test
     * 
     */
    void CalibrationAxe::resetData()
    {
        for (int i = 0; i < NB_LECTURE; i++)
        {
            RawAcc[i] = 0;
        }
    }

    /**
     * @brief Permet de verifier si l'interupteur de selection est activer pour cet axe
     * 
     * @return true  : Lors qu'il est activer et qu'on veux verifier les calcul 
     * @return false : lors qu'il est inactif
     */
    bool CalibrationAxe::checkAxeActive()
    {
        if (digitalRead(pinActiveAxe) == LOW)
        {
            return TRUE;
        }
        return FALSE;
    }

    /**
     * @brief Permet de calculer le poids de facon propotionelle a la rotation et de l'acceleration 
     * 
     * @param rpm 
     */
    void CalibrationAxe::convertirPoidCalcul(uint16_t rpm)
    {
        //La formule est rendu en bas 
        /*float N = rpm;
        float DeltaA = float(Acc.convertRawToGForce(AccMax_raw)-Acc.convertRawToGForce(AccMin_raw));
        float DeltaX = ((60 / N) / 4) * (DeltaA / 2);
        float K = ConstanteDeRapel_K; // Entre 2000 et  3000 // Chaque axe a un balancement unique thechniquement
        float R = 0.03175;
        float Mu = (-K * DeltaX) / (1.1 * R *pow((N/10),2));*/

        //Serial.print("Mu : ");Serial.println(Mu);

        poidCalculer = ((10*pow(float(Acc.convertRawToGForce(AccMax_raw)-Acc.convertRawToGForce(AccMin_raw)),2)) / (1.1*0.03175*pow(rpm/10,2)) * 1000 )-4.0;
        

        

        //Force = 1.1 * Mu * R * (N/10)^2

        //Force = -K deltaX 

        //Mu = (-k * DeltaX) / (1.1 * R *(N/10)^2)



        //K ~= 2000 ou 3000
    }

    /**
     * @brief Permet d'appliquer le offset sur la position de l'axe
     * 
     */
    void CalibrationAxe::appliquerOffeset()
    {
        int offsetMin = 0;
        if (Offset < 0)
        {
            offsetMin = Offset + 50;
        }
        else
        {
            offsetMin = Offset - 50;
        }

        AngleAccMinOffset_0_100 = AngleAccMinInverser + offsetMin; // Offset min different
        AngleAccMaxOffset_0_100 = AngleAccMaxInverser + Offset;
        

        if (AngleAccMinOffset_0_100 >= 100)
        {
            AngleAccMinOffset_0_100 -= 100;
        }
        else if (AngleAccMinOffset_0_100 < 0)
        {
            AngleAccMinOffset_0_100 += 100;
        }

        if (AngleAccMaxOffset_0_100 >= 100)
        {
            AngleAccMaxOffset_0_100 -= 100;
        }
        else if (AngleAccMaxOffset_0_100 < 0)
        {
            AngleAccMaxOffset_0_100 += 100;
        }
    }

    /**
     * @brief Permet d'appliquer le offset sur la position de l'axe
     * 
     */
    void CalibrationAxe::appliquerOffesetCoded()
    {
        int offsetMin = 0;
        if (OffsetCoded < 0)
        {
            offsetMin = OffsetCoded + 50;
        }
        else
        {
            offsetMin = OffsetCoded - 50;
        }

        #if _INVERTION_DONNE_
        AngleAccMinOffset_0_100 = AngleAccMinInverser + offsetMin; // Offset min different
        AngleAccMaxOffset_0_100 = AngleAccMaxInverser + OffsetCoded;
        #else
        AngleAccMinOffset_0_100 =  AngleAccMin_0_100 + offsetMin; // Offset min different
        AngleAccMaxOffset_0_100 = AngleAccMax_0_100 + OffsetCoded;
        #endif

        if (AngleAccMinOffset_0_100 >= 100)
        {
            AngleAccMinOffset_0_100 -= 100;
        }
        else if (AngleAccMinOffset_0_100 < 0)
        {
            AngleAccMinOffset_0_100 += 100;
        }

        if (AngleAccMaxOffset_0_100 >= 100)
        {
            AngleAccMaxOffset_0_100 -= 100;
        }
        else if (AngleAccMaxOffset_0_100 < 0)
        {
            AngleAccMaxOffset_0_100 += 100;
        }
    }

    /**
     * @brief Algoritme qui permet de determiner les valeurs peak negatif(min) et positif(max) 
     *        
     * NOTE : Pour le moment un inversion est effectuer ici il va falloir modifier sa pour avoir 
     *          un systeme plus dynamique qui s'adapte au machine 
     * 
     */
    void CalibrationAxe::algoPeak()
    {
        int i;
        float highestValue = 0;
        float lowestValue = 1027;

        for (i = 0; i < NB_LECTURE; i++)
        {
            if (RawAcc[i] > highestValue)
            {
                highestValue = RawAcc[i];
                AngleAccMax_0_100 = i;
            }

            if (RawAcc[i] < lowestValue)
            {
                if (RawAcc[i] > 0)
                {
                    lowestValue = RawAcc[i];
                    AngleAccMin_0_100 = i;
                }
            }
        }
        AccMin_raw = lowestValue;
        AccMax_raw = highestValue;

        
        //inversion des donnees (dedier pour le premier shaft ) (A MODIFIER)
        AngleAccMinInverser = map(AngleAccMin_0_100,IN_MIN_POSITION,IN_MAX_POSITION,OUT_MIN_POSITION,OUT_MAX_POSITION);
        AngleAccMaxInverser = map(AngleAccMax_0_100,IN_MIN_POSITION,IN_MAX_POSITION,OUT_MIN_POSITION,OUT_MAX_POSITION);

    }


    /**
     * @brief Permet de preparer le data avant d'etre afficher
     *          Et permet de ne pas prendre le valeur extreme ! 
     * 
     * On devrai modifier ici le bon affichage en fonction du type disponible sur la machine ! (A MODIFIER) 
     * 
     */
    void CalibrationAxe::prepareDataAfficher()
    {
        //Preparation de l'angle a afficher
        #if _OFFESET_CODED__ 
        appliquerOffesetCoded(); //Code manuellement dans le code
        #else  
        appliquerOffeset();
        #endif
        
        
        if (AngleAccMax_0_100 < 10 || AngleAccMax_0_100 > 90)
        {
            //Prendre le Min
            afficherDiplayPosition = AngleAccMinOffset_0_100;
        }
        else
        {
            //Prendre la Max
            afficherDiplayPosition = AngleAccMaxOffset_0_100;
        }

        //Preparation du poids a afficher
        if(MasseUnbalance_oz < 0)
        {
            afficherDisplayPoid = 0;
        }
        else
        {
        afficherDisplayPoid = MasseUnbalance_oz * 10; // * 10 pour afficher les decimal de la valeur 
        
        }

        if(afficherDisplayPoid > 999)
        {
            afficherDisplayPoid = 999;
        }

    }


    /**
     * @brief Permet d'afficher les valeurs sur l'afficheur 7 segment dedier a cette axe
     * 
     */
    void CalibrationAxe::displayValue()
    {
        uint8_t buff[4];
        
        //split_4_digit_number() ... 

        //Angle
        split_4_digit_number(afficherDiplayPosition * 3.6, buff); // 3.6 permet de ramener entre 0 -- 360 deg cela fait qu'on est dans une précision de 4 degré par contre 
        //displayPosition.display(0, buff[0]); // (Where,Value)
        displayPosition.display(1, buff[1]);
        displayPosition.display(2, buff[2]);
        displayPosition.display(3, buff[3]);
        //poids
        split_4_digit_number(afficherDisplayPoid, buff);
        displayPoids.display(1, buff[1]);
        displayPoids.display(2, buff[2]);
        displayPoids.display(3, buff[3]);
    }

    /**
     * @brief Permet de faire la conversion de toute les valeurs du test 
     * 
     */
    void CalibrationAxe::allConversion()
    {
        for (int i = 0; i < NB_LECTURE; i++)
        {
            RawAcc[i] = Acc.convertRawToGForce(RawAcc[i]);
        }

        AccMin_raw = Acc.convertRawToGForce(AccMin_raw);
        AccMax_raw = Acc.convertRawToGForce(AccMax_raw);
        AngleAccMin_deg = AngleAccMin_0_100 * (360 / NB_LECTURE);
        AngleAccMax_deg = AngleAccMax_0_100 * (360 / NB_LECTURE);
    }

    
    void CalibrationAxe::convertirPoidEnGrammeCalcul(uint16_t rpm,float diametre)
    {
        float N = rpm;
        float Rad = (rpm/60) *2 *PI;
        float DeltaA = float(Acc.convertRawToGForce(AccMax_raw)-Acc.convertRawToGForce(AccMin_raw));
        float DeltaX = ((60 / N) / 2) * pow((DeltaA / 2),2);
        float K = ConstanteDeRapel_K; // Entre 2000 et  3000 // Chaque axe a un balancement unique thechniquement
        float R = diametre / 2;
        float Mu = (-K * DeltaX) / (1.1 * R *pow((N/10),2)); // kg for now 

        float tata = (DeltaA * K) / ((1.1 * R *pow((N/10),2)));
        Serial.print("TATA TEST : ");Serial.println(tata);
        
        float DeltaX2 = pow(((60.0/(float)rpm)/2.0),2) * (DeltaA / 2); // rpm to s 
        float Mu2 = (400000 * DeltaX2) / (R *pow(Rad,2));
        Serial.print("DeltaX : ");Serial.println(DeltaX);
        Serial.print("R");Serial.println(R);
        Serial.print("N");Serial.println(N);

        /*float DeltaA2 = 0; 
        float Accmax_m_s2 = Acc.convertRawToGForce(AccMax_raw);
        float Accmin_m_s2 = Acc.convertRawToGForce(AccMin_raw);
        if(abs(Accmax_m_s2) > abs(Accmin_m_s2))
        {
            DeltaA2 = Accmax_m_s2;
        }
        else
        {
            DeltaA2 = Accmin_m_s2;
        }

        float test = pow(DeltaA2,3)/6; 
        Serial.print("TEST : -----------------------------------");Serial.println(test);
         float DeltaX2 = ((60 / N) / 2) * pow((DeltaA2),2);
        
         float Mu2 = (-K * DeltaX2) / (1.1 * R *pow((N/10),2)); // kg for now*/ 

        MasseUnbalance_g = Mu * 1000; //EN gramme 
        MasseUnbalance_oz = tata; // en oz  
        MasseUnbalance_g2 = Mu2 * 1000; //EN gramme 
        MasseUnbalance_oz2 = Mu2 * 35.274; // en oz  
    }