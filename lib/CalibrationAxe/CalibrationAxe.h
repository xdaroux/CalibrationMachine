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

#define _DEBUG_N1_ 1
#define _OFFESET_CODED__ 1 
#define _INVERTION_DONNE_ 0

#define NB_LECTURE 100
#define NB_TEST 20
#define TRUE 1
#define FALSE 0
#define LUMINOSITE 2
/*****************************************************************/
// Modification du OUT Selon l'affichage actuelle sur la machine 
/*****************************************************************/
#define IN_MIN_POSITION 0
#define IN_MAX_POSITION 100
#define OUT_MIN_POSITION 100
#define OUT_MAX_POSITION 0
/***********************************************************/
class CalibrationAxe
{
private:
    uint8_t pinActiveAxe;

public:
    String NomAxe;
    Accelerometre Acc;
    volatile uint8_t FlagNewRotation; 
    float RawAcc[NB_LECTURE];
    uint32_t NbLectureTotal;

    float AccMax_raw;
    uint16_t AngleAccMax_0_100;
    int16_t AngleAccMaxOffset_0_100;
    int16_t AngleAccMaxOffset_0_360;
    float AngleAccMax_deg;

    float AccMin_raw;
    uint16_t AngleAccMin_0_100;
    int16_t AngleAccMinOffset_0_100;
    int16_t AngleAccMinOffset_0_360;
    float AngleAccMin_deg;

    int16_t AngleAccMinInverser;
    int16_t AngleAccMaxInverser; 


    TM1637 displayPoids;
    TM1637 displayPosition;
    uint16_t afficherDiplayPosition;
    uint16_t afficherDisplayPoid;
    bool blinkEtat;

    int16_t Offset;
    int16_t OffsetCoded;
    uint8_t pinOffset;

    bool erreur;

    float ConstanteDeRapel_K; //Newton * Metre
    float poidCalculer;
    float MasseUnbalance_g;
    float MasseUnbalance_oz;
    float MasseUnbalance_g2;
    float MasseUnbalance_oz2;

    

void CalibrationAxe::init(uint8_t pinAnalog, uint16_t zero, float span, 
                            uint8_t pinDigital, String nomAxe, uint8_t pinDigitalDisplayPositionClk, 
                            uint8_t pinDigitalDisplayPositionData,uint8_t pinDigitalDisplayPoidsClk,uint8_t pinDigitalDisplayPoidsData, uint8_t pinAnalogOffset,float constanteDeRapel, int16_t offsetCoded);
 

void init(uint8_t pinAnalog, uint16_t zero, float span, uint8_t pinDigital,
                            String nomAxe, uint8_t pinDigitalDisplayClk, uint8_t pinDigitalDisplayData,
                            uint8_t pinAnalogOffset,float constanteDeRapel);

void calibrationZeroAcc();
void afficherCalibZero();
void afficherInfoTest();

void test(uint16_t rpm, float diametre);
void afficherTest(String info);


void afficherPeak();
void afficherInfoCalibPoids();
void allConversion();

bool checkErreur();
void blinkDisplay();

void editOffset();

private : 

void resetData();
bool checkAxeActive(); 

void algoPeak();
void appliquerOffeset();
void appliquerOffesetCoded();
void convertirPoidCalcul(uint16_t rpm);
void prepareDataAfficher(); 
void changeScaleData_0_360(); 
void displayValue();
void convertirPoidEnGrammeCalcul(uint16_t rpm, float diametre);

    // TODO detection forme bizzard genre harmonique peut le faire avec deriver (trouver deux max local)


    // TODO calcul de la masse en fonction du RPM et du diametre
};