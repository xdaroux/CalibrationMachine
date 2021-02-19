/**
 * @file DiametreShaft.h
 * @author Dartagnan Leonard (Dartagnan.leonard@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2021-01-11
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <Arduino.h>
#include "displayData.h"

#define DIAMETRE_MIN_PO 1.0
#define DIAMETRE_MAX_PO 6.0
#define RAW_MIN 0.0
#define RAW_MAX 1023.0

#define TIME_ENTRE_LECTURE 1000

class DiametreShaft
{
private:

    uint8_t pinDia;
    uint32_t TimerLecture;

public:

    uint32_t rawDia;
    float Diametre_po;
    float Diametre_m;
    float Rayon_po;
    float Rayon_m;
    uint32_t DiamDisplay;


    void init(uint8_t pinAnalog)
    {
        pinDia = pinAnalog;
        lectureDiametre();
    }

    void main(TM1637 display)
    {
            TimerLecture = millis();
            lectureDiametre();
            displayDiametre(display);  
    }

    void displayDiametre(TM1637 display)
    {
        uptade_display_diametreShaft(display);
    }

    void lectureDiametre()
    {
        for(int i = 0; i < 100; i++)
        {
        rawDia += analogRead(pinDia);
        }
        rawDia = rawDia / 100;
        Diametre_po = convert_raw_po(rawDia);
        Diametre_m = convert_po_meter(Diametre_po);
        Rayon_po = Diametre_po / 2;
        Rayon_m = Diametre_m /2;
        DiamDisplay = Diametre_po * 100;
    }

    void uptade_display_diametreShaft(TM1637 DiamShaftDisplay)
    {
        uint8_t buff[4];

        split_4_digit_number(DiamDisplay, buff);

        DiamShaftDisplay.display(0, buff[0]); // (Where,Value)
        DiamShaftDisplay.display(1, buff[1]);
        DiamShaftDisplay.display(2, buff[2]);
        DiamShaftDisplay.display(3, buff[3]);

    }



    float convert_raw_po(uint16_t rawValue)
    {
       return mapFloat(rawValue,RAW_MIN,RAW_MAX,DIAMETRE_MIN_PO,DIAMETRE_MAX_PO); 
    }

    float convert_po_meter(float distance_po)
    {
        return (distance_po * 0.0254);
    }

    float mapFloat(float x , float in_min , float in_max, float out_min, float out_max)
    { 
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }



};