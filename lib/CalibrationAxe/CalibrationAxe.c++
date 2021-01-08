#include "CalibrationAxe.h"

 void CalibrationAxe::init(uint8_t pinAnalog, uint16_t zero, float span, uint8_t pinDigital, String nomAxe, uint8_t pinDigitalDisplayClk, uint8_t pinDigitalDisplayData, uint8_t pinAnalogOffset)
    {
        Acc.init(pinAnalog, zero, span);
        NomAxe = nomAxe;
        pinActiveAxe = pinDigital;
        pinMode(pinActiveAxe, INPUT_PULLUP);
        resetData();
        display.init(pinDigitalDisplayClk, pinDigitalDisplayData);
        display.set(LUMINOSITE);
        display.point(POINT_ON);
        pinOffset = pinAnalogOffset;
        blinkEtat = 0;
        erreur = 0;
        AngleAccMinOffset_0_100 = 0;
        AngleAccMaxOffset_0_100 = 0;
    }
    void CalibrationAxe::resetData()
    {
        for (int i = 0; i < NB_LECTURE; i++)
        {
            RawAcc[i] = 0;
        }
    }

    void CalibrationAxe::test(uint16_t rpm)
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
            
            algoPeak(); //OBG obligatoire
            //allConversion();
            convertirPoidCalcul(rpm);
            prepareDataAfficher(); //OBG
            displayValue(); //OBG

            
            afficherInfoTest();
            
        }
    }

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

    bool CalibrationAxe::checkAxeActive()
    {
        if (digitalRead(pinActiveAxe) == LOW)
        {
            return TRUE;
        }
        return FALSE;
    }

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

        AngleAccMinInverser = map(AngleAccMin_0_100,0,100,100,0);
        AngleAccMaxInverser = map(AngleAccMax_0_100,0,100,100,0);
    }

    void CalibrationAxe::afficherPeak()
    {
        Serial.println(NomAxe);
        Serial.print("Peak Min : ");
        Serial.println(AccMin_raw);
        Serial.print("Angle Min 0 - 99  : ");
        Serial.println(AngleAccMin_0_100);
        Serial.print("Peak Max : ");
        Serial.println(AccMax_raw);
        Serial.print("Angle Max 0 - 99  : ");
        Serial.println(AngleAccMax_0_100);
        Serial.println("==================================");
        Serial.print("Afficher G force Min : ");Serial.println(Acc.convertRawToGForce(AccMin_raw));
        Serial.print("Afficher G force Max : ");Serial.println(Acc.convertRawToGForce(AccMax_raw));
        Serial.print("Afficher G force Diff : ");Serial.println(Acc.convertRawToGForce(AccMax_raw) - Acc.convertRawToGForce(AccMin_raw) );
        Serial.println("==================================");
    }

    void CalibrationAxe::afficherInfoCalibPoids()
    {
        Serial.println(AccMin_raw);
        Serial.println(AccMax_raw);
        Serial.println(abs(AccMax_raw-AccMin_raw));
        Serial.println(Acc.convertRawToGForce(AccMin_raw));
        Serial.println(Acc.convertRawToGForce(AccMax_raw));
        Serial.println(Acc.convertRawToGForce(AccMax_raw)-Acc.convertRawToGForce(AccMin_raw));
    }

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

    bool CalibrationAxe::checkErreur()
    {
        uint16_t tmp = abs(AngleAccMaxOffset_0_100-AngleAccMinOffset_0_100);
        if( tmp > 15 && tmp < 75) // Valeur agle diff acceptable 
        {
            erreur = TRUE;
        }
        else
        {
            erreur = FALSE;
        }

        return erreur;   
    }

    void CalibrationAxe::prepareDataAfficher()
    {
        //Preparation de l'angle a afficher
        appliquerOffeset();
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
        if(poidCalculer < 0)
        {
            afficherDisplayPoid = 0;
        }
        else
        {
        afficherDisplayPoid = poidCalculer;
        }

        if(afficherDisplayPoid > 99)
        {
            afficherDisplayPoid = 99;
        }
    }

    void CalibrationAxe::displayValue()
    {
        uint8_t buff[2];
        
        //Angle
        split_2_digit_number(afficherDiplayPosition, buff);
        display.display(0, buff[0]); // (Where,Value)
        display.display(1, buff[1]);
        //poids
        split_2_digit_number(afficherDisplayPoid, buff);
        display.display(2, buff[0]);
        display.display(3, buff[1]);
    }

    void CalibrationAxe::blinkDisplay()
    {
        if(blinkEtat == 0)
        {
            display.clearDisplay();
            blinkEtat = 1;
        }
        else
        {
            displayValue();
            blinkEtat = 0;
        }
        
    }

    void CalibrationAxe::editOffset()
    {
        int16_t offset;
        uint8_t buff[2];
        offset = map(analogRead(pinOffset), 0, 1023, -50, 50);

        split_2_digit_number(abs(Offset), buff);

        display.display(2, buff[0]);
        display.display(3, buff[1]);
        if (Offset < 0)
        {
            display.displaySeg(1, 0b01000000);
        }
        else
        {
            display.displaySeg(1, 0b00000000);
        }

        if (offset != Offset)
        {
            Offset = offset;
        }
    }

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


    void CalibrationAxe::calibrationZeroAcc()
    {
        Acc.calibrationZero();
        afficherCalibZero(); 
    }

    void CalibrationAxe::afficherCalibZero()
    {
        Serial.println("=================Calibration Zero=======================");
        Serial.println(NomAxe);
        Serial.print("Zero : ");Serial.println(Acc.Zero);
        Serial.println("========================================================");
    }

    void CalibrationAxe::afficherInfoTest()
    {
            //afficherTest("Test Apres Moyenne");
            afficherPeak();
           // afficherInfoCalibPoids();
            Serial.print("Valeur avec offset min :");Serial.println(AngleAccMinOffset_0_100);
            Serial.print("Valeur avec offset Max :");Serial.println(AngleAccMaxOffset_0_100);
            
    }

    void CalibrationAxe::convertirPoidCalcul(uint16_t rpm)
    {
        poidCalculer = ((10*pow(float(Acc.convertRawToGForce(AccMax_raw)-Acc.convertRawToGForce(AccMin_raw)),2)) / (1.1*0.03175*pow(rpm/10,2)) * 1000 )-4.0;
        Serial.print("POIDS CALCULER : "); Serial.println(poidCalculer);
    }