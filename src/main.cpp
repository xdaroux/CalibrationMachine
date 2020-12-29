#include <Arduino.h>

#include "CalibrationAxe.h"
#include "RPM_NEW.h"
#include "displayData.h"
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

rpm_t Rpm;
unsigned long timerAfficherRPM = millis();
uint32_t timerOldButton = millis();

#define RPM_DIO 30
#define RPM_CLK 31
TM1637 RpmDisplay;

CalibrationAxe CalibAxe_1;
#define pinACC_1 A0
#define pinACTIVE_AXE_1 47
#define ACC_1_ZERO 515
#define ACC_1_SPAN 102
#define pinDISPLAY_1_DIO 28
#define pinDISPLAY_1_CLK 29

CalibrationAxe CalibAxe_2;
#define pinACC_2 A1
#define pinACTIVE_AXE_2 49
#define ACC_2_ZERO 470
#define ACC_2_SPAN 102
#define pinDISPLAY_2_DIO 26
#define pinDISPLAY_2_CLK 27

CalibrationAxe CalibAxe_3;
#define pinACC_3 A2
#define pinACTIVE_AXE_3 51
#define ACC_3_ZERO 505
#define ACC_3_SPAN 102
#define pinDISPLAY_3_DIO 24
#define pinDISPLAY_3_CLK 25

CalibrationAxe CalibAxe_4;
#define pinACC_4 A3
#define pinACTIVE_AXE_4 53
#define ACC_4_ZERO 481
#define ACC_4_SPAN 102
#define pinDISPLAY_4_DIO 22
#define pinDISPLAY_4_CLK 23

#define pinSwitch 13
#define pinInterrupt 2
#define pinOFFSET A15

//Declaration Fonction//
void blink();
bool oldTimerButton(uint16_t timer);
void clearAllDisplay();
void editAllOffset();
void calibAllAxeZero();

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  analogReference(EXTERNAL); // use AREF for reference voltage

  rpm_init(&Rpm);
  RpmDisplay.init(RPM_CLK, RPM_DIO);
  RpmDisplay.set(LUMINOSITE);

  CalibAxe_1.init(pinACC_1, ACC_1_ZERO, ACC_1_SPAN, pinACTIVE_AXE_1, "AXE 1", pinDISPLAY_1_CLK, pinDISPLAY_1_DIO, pinOFFSET);
  CalibAxe_2.init(pinACC_2, ACC_2_ZERO, ACC_2_SPAN, pinACTIVE_AXE_2, "AXE 2", pinDISPLAY_2_CLK, pinDISPLAY_2_DIO, pinOFFSET);
  CalibAxe_3.init(pinACC_3, ACC_3_ZERO, ACC_3_SPAN, pinACTIVE_AXE_3, "AXE 3", pinDISPLAY_3_CLK, pinDISPLAY_3_DIO, pinOFFSET);
  CalibAxe_4.init(pinACC_4, ACC_4_ZERO, ACC_4_SPAN, pinACTIVE_AXE_4, "AXE 4", pinDISPLAY_4_CLK, pinDISPLAY_4_DIO, pinOFFSET);

  //calibAllAxeZero();

  attachInterrupt(digitalPinToInterrupt(pinInterrupt), blink, FALLING);
  pinMode(pinSwitch, INPUT_PULLUP);

  //Do it once
  editAllOffset();

  clearAllDisplay();
}

void loop()
{
  //Serial.println(CalibAxe_4.Acc.read());
  // put your main code here, to run repeatedly:
  if (millis() - timerAfficherRPM > 500)
  {
    rpm_calcul(&Rpm);
    //Serial.print("RPM : ");
    //Serial.println(Rpm.rpm);
    //Serial.println(analogRead(A0));
    uptade_display_rpm(RpmDisplay, Rpm.rpm);
    timerAfficherRPM = millis();
    //
  }

  switch (Etat)
  {
  case INIT:
    Serial.println("ETAT : INIT");
    Etat = ATTENTE;
    break;

  case EDIT:
    editAllOffset();
    if (oldTimerButton(2000))
    {
      Etat = ATTENTE;
      clearAllDisplay();
    }
    break;

  case ATTENTE:

    if (Rpm.rpm < 100)
    {
      if (oldTimerButton(5000))
      {
        Etat = EDIT;
        clearAllDisplay();
      }
    }

    if (Rpm.rpm > 100 && digitalRead(pinSwitch) == LOW)
    {
      Etat = TEST_AXE_1;
      clearAllDisplay();
    }
    break;

  case TEST_AXE_1:
    Serial.println("ETAT : TEST AXE 1");
    //Flag 1

    CalibAxe_1.test(Rpm.rpm);
    //Affichage

    Etat = TEST_AXE_2;
    break;

  case TEST_AXE_2:
    Serial.println("ETAT : TEST AXE 2");
    //Flag 2
    //Flag 1

    CalibAxe_2.test(Rpm.rpm);
    //Affichage
    Etat = TEST_AXE_3;

    break;

  case TEST_AXE_3:
    Serial.println("ETAT : TEST AXE 3");
    //Flag 3
    //Flag 1

    CalibAxe_3.test(Rpm.rpm);
    //Affichage
    Etat = TEST_AXE_4;

    break;

  case TEST_AXE_4:
    Serial.println("ETAT : TEST AXE 4");
    //Flag 4
    //Flag 1
    CalibAxe_4.test(Rpm.rpm);
    //Affichage
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
    //Lecture de RPM
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

void clearAllDisplay()
{
  CalibAxe_1.display.clearDisplay();
  CalibAxe_2.display.clearDisplay();
  CalibAxe_3.display.clearDisplay();
  CalibAxe_4.display.clearDisplay();
}

void editAllOffset()
{
  CalibAxe_1.editOffset();
  CalibAxe_2.editOffset();
  CalibAxe_3.editOffset();
  CalibAxe_4.editOffset();
}

void calibAllAxeZero()
{
  CalibAxe_1.calibrationZeroAcc();
  CalibAxe_2.calibrationZeroAcc();
  CalibAxe_3.calibrationZeroAcc();
  CalibAxe_4.calibrationZeroAcc();
}