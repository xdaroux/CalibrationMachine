/**
 * @file CalibrationAxe.h
 * @author Dartagnan Leonard (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2020-12-21
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include <Arduino.h>
#include "Accelerometre.h"
#include <TM1637.h>
#include "displayData.h"

#define NB_LECTURE 100
#define NB_TEST 20
#define TRUE 1
#define FALSE 0
#define LUMINOSITE 2

class CalibrationAxe
{

    uint8_t pinActiveAxe;

public:
    String NomAxe;
    Accelerometre Acc;
    volatile uint8_t FlagNewRotation; // Pourrais aussi juste passer l'adresse de la fonction
    float RawAcc[NB_LECTURE];
    uint32_t NbLectureTotal;

    float AccMax_raw;
    uint16_t AngleAccMax_0_100;
    int16_t AngleAccMaxOffset_0_100;
    float AngleAccMax_deg;

    float AccMin_raw;
    uint16_t AngleAccMin_0_100;
    int16_t AngleAccMinOffset_0_100;
    float AngleAccMin_deg;

    int16_t AngleAccMinInverser;
    int16_t AngleAccMaxInverser; 


    TM1637 display;
    uint16_t afficherDiplayPosition;
    uint16_t afficherDisplayPoid;
    bool blinkEtat;

    int16_t Offset;
    uint8_t pinOffset;

    bool erreur;



    void init(uint8_t pinAnalog, uint16_t zero, float span, uint8_t pinDigital, String nomAxe, uint8_t pinDigitalDisplayClk, uint8_t pinDigitalDisplayData, uint8_t pinAnalogOffset)
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
    void resetData()
    {
        for (int i = 0; i < NB_LECTURE; i++)
        {
            RawAcc[i] = 0;
        }
    }

    void test(uint16_t rpm)
    {
        // reset 
        AngleAccMinOffset_0_100 = 0;
        AngleAccMaxOffset_0_100 = 0;
        erreur = 0;

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
            afficherTest("Test Apres Moyenne");
            algoPeak();
            afficherPeak();
            afficherInfoCalibPoids();
            allConversion();
            prepareDataAfficher();
            displayValue();
            Serial.print("Valeur avec offset min :");Serial.println(AngleAccMinOffset_0_100);
            Serial.print("Valeur avec offset Max :");Serial.println(AngleAccMaxOffset_0_100);
        }
    }

    void afficherTest(String info)
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

    bool checkAxeActive()
    {
        if (digitalRead(pinActiveAxe) == LOW)
        {
            return TRUE;
        }
        return FALSE;
    }

    void algoPeak()
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

    void afficherPeak()
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
    }

    void afficherInfoCalibPoids()
    {
        Serial.println(AccMin_raw);
        Serial.println(AccMax_raw);
        Serial.println(abs(AccMax_raw-AccMin_raw));
        Serial.println(Acc.convertRawToGForce(AccMin_raw));
        Serial.println(Acc.convertRawToGForce(AccMax_raw));
        Serial.println(Acc.convertRawToGForce(AccMax_raw)-Acc.convertRawToGForce(AccMin_raw));
    }

    void allConversion()
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
    bool checkErreur()
    {
        if(abs(AngleAccMaxOffset_0_100-AngleAccMinOffset_0_100) > 15) // 
        {
            erreur = TRUE;
        }
        else
        {
            erreur = FALSE;
        }

        return erreur;   
    }

    void prepareDataAfficher()
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
        afficherDisplayPoid = (AccMax_raw - AccMin_raw)*2.5;

        if(afficherDisplayPoid > 99)
        {
            afficherDisplayPoid = 99;
        }
    }

    void displayValue()
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

    void blinkDisplay()
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

    void editOffset()
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

    void appliquerOffeset()
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


    void calibrationZeroAcc()
    {
        Acc.calibrationZero();
        afficherCalibZero();
        
    }

    void afficherCalibZero()
    {
        Serial.println("=================Calibration Zero=======================");
        Serial.println(NomAxe);
        Serial.print("Zero : ");Serial.println(Acc.Zero);
        Serial.println("========================================================");
    }

    // TODO Filtrage @ frequence du RPM

    // TODO AngleDiff ~ 180

    // TODO detection forme bizzard genre harmonique peut le faire avec deriver (trouver deux max local)

    // TODO Blink Display quand erreur differente speed !!!

    // TODO calcul de la masse en fonction du RPM et du diametre
};