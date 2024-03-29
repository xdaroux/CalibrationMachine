#include <Arduino.h>

#include "CalibrationAxe.h"
#include "RPM_NEW.h"
#include "displayData.h"
#include "DiametreShaft.h"

#define _DEBUG_N1_ 0

//Definition des Type//
enum ETAT
{
  INIT,
  ATTENTE,
  TEST_AXE_1,
  TEST_AXE_2,
  TEST_AXE_3,
  TEST_AXE_4,
  EDIT
};

//Variable GLobal//
ETAT Etat = INIT;

DiametreShaft DiamShaft;
#define pinDiamShaft A14

rpm_t Rpm;
#define MIN_ACTIF_RPM 100
unsigned long timerAfficherRPM = millis();
uint32_t timerOldButton = millis();

uint32_t timeOlderErreur = millis();

#define RPM_DIO 30
#define RPM_CLK 31
TM1637 RpmDisplay;

CalibrationAxe CalibAxe_1;
#define NOM_AXE_1 "REAR"
#define pinACC_1 A0
#define pinACTIVE_AXE_1 47
#define ACC_1_ZERO 507//484 // 515 sans lecture 
#define ACC_1_SPAN 102
#define CONSTANTE_DE_RAPEL_K_1 -2850
#define pinDISPLAY_1_DIO 28
#define pinDISPLAY_1_CLK 29

CalibrationAxe CalibAxe_2;
#define NOM_AXE_2 "AXE 2"
#define pinACC_2 A1
#define pinACTIVE_AXE_2 49
#define ACC_2_ZERO 501//470  //500 sans lecture 
#define ACC_2_SPAN 102
#define CONSTANTE_DE_RAPEL_K_2 -1425
#define pinDISPLAY_2_DIO 26
#define pinDISPLAY_2_CLK 27

CalibrationAxe CalibAxe_3;
#define NOM_AXE_3 "AXE 1"
#define pinACC_3 A2
#define pinACTIVE_AXE_3 51
#define ACC_3_ZERO 500//475 // 505 sans lecture 
#define ACC_3_SPAN 102
#define CONSTANTE_DE_RAPEL_K_3 -1425
#define pinDISPLAY_3_DIO 24
#define pinDISPLAY_3_CLK 25

CalibrationAxe CalibAxe_4;
#define NOM_AXE_4 "DRIVE"
#define pinACC_4 A3
#define pinACTIVE_AXE_4 53
#define ACC_4_ZERO 508//481  // 512 sans lecture
#define ACC_4_SPAN 102
#define CONSTANTE_DE_RAPEL_K_4 -2850
#define pinDISPLAY_4_DIO 22
#define pinDISPLAY_4_CLK 23

#define pinSwitch 13
#define pinInterrupt 2
#define pinOFFSET A15

//Constante General 
#define TIME_UPTADTE_RPM 500
#define TIME_BLINK_ERREUR 500
#define TIME_OLD_BUTTON_ATTENTE 2000
#define TIME_OLD_BUTTON_EDIT 5000

//Declaration Fonction//
void blink();
bool oldTimerButton(uint16_t timer);
void clearAllDisplay();
void editAllOffset();
void calibAllAxeZero();
void checkAllErreur();

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  analogReference(EXTERNAL); // use AREF for reference voltage of the accelerometer 

  /*---------------RPM---------------*/
  rpm_init(&Rpm);
  RpmDisplay.init(RPM_CLK, RPM_DIO);
  RpmDisplay.set(LUMINOSITE);
  /*------------- END RPM-------------*/

  /*---------------DiamShaft---------------*/
  DiamShaft.init(pinDiamShaft);
  /*-------------END DiamShaft-------------*/

  /*-----------------Calibration Axe-----------------*/
  CalibAxe_1.init(pinACC_1, ACC_1_ZERO, ACC_1_SPAN, pinACTIVE_AXE_1, NOM_AXE_1, pinDISPLAY_1_CLK, pinDISPLAY_1_DIO, pinOFFSET,CONSTANTE_DE_RAPEL_K_1);
  CalibAxe_2.init(pinACC_2, ACC_2_ZERO, ACC_2_SPAN, pinACTIVE_AXE_2, NOM_AXE_2, pinDISPLAY_2_CLK, pinDISPLAY_2_DIO, pinOFFSET,CONSTANTE_DE_RAPEL_K_2);
  CalibAxe_3.init(pinACC_3, ACC_3_ZERO, ACC_3_SPAN, pinACTIVE_AXE_3, NOM_AXE_3, pinDISPLAY_3_CLK, pinDISPLAY_3_DIO, pinOFFSET,CONSTANTE_DE_RAPEL_K_3);
  CalibAxe_4.init(pinACC_4, ACC_4_ZERO, ACC_4_SPAN, pinACTIVE_AXE_4, NOM_AXE_4, pinDISPLAY_4_CLK, pinDISPLAY_4_DIO, pinOFFSET,CONSTANTE_DE_RAPEL_K_4);
  /*---------------END Calibration Axe---------------*/


  //calibAllAxeZero(); // permet de faire la calibration des axe et d'afficher les informations dans la console (PORT SERIE)

  attachInterrupt(digitalPinToInterrupt(pinInterrupt), blink, FALLING);
  pinMode(pinSwitch, INPUT_PULLUP);

  /*----------------OFFSET-----------------*/
  editAllOffset();
  /*-------------- END OFFSET---------------*/

  
  clearAllDisplay();
}

void loop()
{
  if (millis() - timerAfficherRPM > TIME_UPTADTE_RPM)
  {
    rpm_calcul(&Rpm);
    uptade_display_rpm(RpmDisplay, Rpm.rpm);
    timerAfficherRPM = millis();
  }


  
  switch (Etat)
  {
  case INIT:
    Serial.println("ETAT : INIT");
    Etat = ATTENTE;
    break;

  case EDIT:
  RpmDisplay.point(POINT_ON);
  while(oldTimerButton(TIME_OLD_BUTTON_ATTENTE) != TRUE)
  {
    editAllOffset();
    DiamShaft.main(RpmDisplay);
  }
  RpmDisplay.point(POINT_OFF);
  RpmDisplay.clearDisplay();
  clearAllDisplay();
  Etat = ATTENTE;
    break;

  case ATTENTE:

    
    if (Rpm.rpm < MIN_ACTIF_RPM)
    {
      if (oldTimerButton(TIME_OLD_BUTTON_EDIT))
      {
        Etat = EDIT;
        clearAllDisplay();
      }
    }


    
    if (Rpm.rpm > MIN_ACTIF_RPM && digitalRead(pinSwitch) == LOW)
    {
      Etat = TEST_AXE_1;
      clearAllDisplay();
    }

    if(millis() - timeOlderErreur > TIME_BLINK_ERREUR)
    {
      checkAllErreur();
      timeOlderErreur = millis();
    }
    break;

  case TEST_AXE_1:
  #if _DEBUG_N1_
    Serial.println("ETAT : TEST AXE 1");
  #endif
    CalibAxe_1.test(Rpm.rpm,DiamShaft.Diametre_m);
    Etat = TEST_AXE_2;
    break;

  case TEST_AXE_2:
  #if _DEBUG_N1_
    Serial.println("ETAT : TEST AXE 2");
  #endif
    CalibAxe_2.test(Rpm.rpm,DiamShaft.Diametre_m);
    Etat = TEST_AXE_3;
    break;

  case TEST_AXE_3:
  #if _DEBUG_N1_
    Serial.println("ETAT : TEST AXE 3");
  #endif
    CalibAxe_3.test(Rpm.rpm,DiamShaft.Diametre_m);
    Etat = TEST_AXE_4;
    break;

  case TEST_AXE_4:
  #if _DEBUG_N1_
    Serial.println("ETAT : TEST AXE 4");
  #endif
    CalibAxe_4.test(Rpm.rpm,DiamShaft.Diametre_m);
    Etat = INIT;
    break;
  }
}

void blink()
{
  switch (Etat)
  {
  case INIT:
    break;

  case ATTENTE:
    break;

  case TEST_AXE_1:
    //Flag 1
    CalibAxe_1.FlagNewRotation++;
    break;

  case TEST_AXE_2:
    //Flag 2
    CalibAxe_2.FlagNewRotation++;
    break;

  case TEST_AXE_3:
    //Flag 3
    CalibAxe_3.FlagNewRotation++;
    break;

  case TEST_AXE_4:
    //Flag 4
    CalibAxe_4.FlagNewRotation++;
    break;
  case EDIT:
    break;
  }
  //Lecture de RPM
  Rpm.timerOlder = Rpm.timer;
  Rpm.timer = micros();
}

bool oldTimerButton(uint16_t timer)
{
  if (digitalRead(pinSwitch) == LOW)
  {
  }
  else
  {
    timerOldButton = millis();
  }

  if (millis() - timerOldButton > timer)
  {
    timerOldButton = millis();
    return TRUE;
  }

  return FALSE;
}

/**
 * @brief Permet de suprimer l'affichage des tous les afficheurs 7 segment
 *        dedier a l'affichage de la position et du poids
 * 
 */
void clearAllDisplay()
{
  CalibAxe_1.display.clearDisplay();
  CalibAxe_2.display.clearDisplay();
  CalibAxe_3.display.clearDisplay();
  CalibAxe_4.display.clearDisplay();
}

/**
 * @brief Permet de lire la valeur du offset et de l'appliquer a tout les 
 *        axes de la calibrationMachine 
 *  
 */
void editAllOffset()
{
  CalibAxe_1.editOffset();
  CalibAxe_2.editOffset();
  CalibAxe_3.editOffset();
  CalibAxe_4.editOffset();
}

/**
 * @brief Permet de faire la calibration du ZERO de tous les axes
 *        le shaft ne dois pas etre en mouvement
 * 
 */
void calibAllAxeZero()
{
  CalibAxe_1.calibrationZeroAcc();
  CalibAxe_2.calibrationZeroAcc();
  CalibAxe_3.calibrationZeroAcc();
  CalibAxe_4.calibrationZeroAcc();
}

/**
 * @brief Permet de verifier si un erreur a ete detecter sur tout les axes.
 *        Fait blinker l'affichage si une erreur est detecter
 * 
 */
void checkAllErreur()
{
  if(CalibAxe_1.checkErreur())
  {
    CalibAxe_1.blinkDisplay();
  }

  if(CalibAxe_2.checkErreur())
  {
    CalibAxe_2.blinkDisplay();
  }

  if(CalibAxe_3.checkErreur())
  {
    CalibAxe_3.blinkDisplay();
  }

  if(CalibAxe_4.checkErreur())
  {
    CalibAxe_4.blinkDisplay();
  }
}