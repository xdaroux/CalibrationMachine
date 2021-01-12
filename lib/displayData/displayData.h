
/*
*   Processeur : Arduino Mega or Mega 2560
*   Pin Configuration : 
*
*   display 1 : rpm
*               DIO : 42
*               CLK : 43 
*
*   display 2 : acc0
*               DIO : 52
*               CLK : 53
*
*   display 3 : acc1
*               DIO : 50
*               CLK : 51 
*
*   display 4 : acc2
*               DIO : 48
*               CLK : 49
*
*   display 4 : acc3
*               DIO : 46
*               CLK : 47
*
*/
#pragma once

#include <Arduino.h>
#include <TM1637.h>
#include <string.h>

//#include <TM1637Display.h>

#define LUMINOSITE 2 // Echelle de 0 a 7

#define ERREUR 250

void split_2_digit_number(uint8_t num, uint8_t *buff);
void uptade_display_acc(TM1637 acc, uint8_t angle, uint8_t poids);
void split_4_digit_number(uint32_t num, uint8_t *buff);
void uptade_display_rpm(TM1637 rpmDisplay, uint32_t rpm);
void uptade_display_diametreShaft(TM1637 DiamShaftDisplay, float diamShaft);
