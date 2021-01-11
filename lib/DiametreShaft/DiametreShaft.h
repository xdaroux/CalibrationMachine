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

#define DIAMETRE_MIN_PO 1.0
#define DIAMETRE_MAX_PO 6.0
#define RAW_MIN 0.0
#define RAW_MAX 1023.0

class DiametreShaft
{
private:

    uint8_t pinDia;

public:

    uint16_t rawDia;
    float Diametre_po;
    float Diametre_m;
    float Rayon_po;
    float Rayon_m;


    void init(uint8_t pinAnalog)
    {
        pinDia = pinAnalog;
    }

    void lectureDiametre()
    {
        rawDia = analogRead(pinDia);
        Diametre_po = convert_raw_po(rawDia);
        Diametre_m = convert_po_meter(Diametre_po);
        Rayon_po = Diametre_po / 2;
        Rayon_m = Diametre_m /2;
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