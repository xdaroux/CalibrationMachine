#include "displayData.h"

//variable global au fichier

void uptade_display_acc(TM1637 acc, uint8_t angle, uint8_t poids)
{
  uint8_t buff[2];
  //Angle

  split_2_digit_number(angle, buff);
  acc.display(0, buff[0]); // (Where,Value)
  acc.display(1, buff[1]);

  //poids
  split_2_digit_number(poids, buff);
  acc.display(2, buff[0]);
  acc.display(3, buff[1]);
}

void split_2_digit_number(uint8_t num, uint8_t *buff)
{
  if (num <= 99)
  {
    buff[0] = num / 10; // dizaine
    buff[1] = num % 10; // unite
  }
}
void split_4_digit_number(uint32_t num, uint8_t *buff)
{
  uint32_t tmpNum = num;
  if (num <= 9999)
  {
    tmpNum = num * 0.01;                    //faire un 2 digit number millier et centaine
    split_2_digit_number(tmpNum, buff);     //[0]:Millier, [1] : Centaine
    tmpNum = num % 100;                     // Faire un 2 digit number Dizaine et unite
    split_2_digit_number(tmpNum, &buff[2]); //[2]:Dizaine, [3] : unite
    /*
    buff[0] = tmpNum *0.001; //millier
    tmpNum = tmpNum *0.1 ;// faire un 3 digit number 
    buff[1] = tmpNum *0.01; //Centaine 
    tmpNum = tmpNum *0.1; // faire un 2 digit number 
    buff[2] = tmpNum *0.1;// Dizaine 
    */
  }
}
void uptade_display_rpm(TM1637 rpmDisplay, uint32_t rpm)
{
  uint8_t buff[4];
  //RPM
  split_4_digit_number(rpm, buff);
  rpmDisplay.display(0, buff[0]); // (Where,Value)
  rpmDisplay.display(1, buff[1]);
  rpmDisplay.display(2, buff[2]);
  rpmDisplay.display(3, buff[3]);
}
