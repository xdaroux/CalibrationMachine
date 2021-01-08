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

    float poidCalculer;


void init(uint8_t pinAnalog, uint16_t zero, float span, uint8_t pinDigital,
                            String nomAxe, uint8_t pinDigitalDisplayClk, uint8_t pinDigitalDisplayData,
                            uint8_t pinAnalogOffset);

void resetData();
void test(uint16_t rpm);
void afficherTest(String info);
bool checkAxeActive();
void algoPeak();
void afficherPeak();
void afficherInfoCalibPoids();
void allConversion();
bool checkErreur();
void prepareDataAfficher();
void displayValue();
void blinkDisplay();
void editOffset();
void appliquerOffeset();
void calibrationZeroAcc();
void afficherCalibZero();
void afficherInfoTest();
void convertirPoidCalcul(uint16_t rpm);
   

    // TODO Filtrage @ frequence du RPM

    // TODO AngleDiff ~ 180

    // TODO detection forme bizzard genre harmonique peut le faire avec deriver (trouver deux max local)


    // TODO calcul de la masse en fonction du RPM et du diametre
};