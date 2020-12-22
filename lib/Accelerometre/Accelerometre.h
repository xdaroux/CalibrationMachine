/**
 * @file Accelerometre.h
 * @author Dartagnan Leonard (Dartagnan.leonard@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-12-21
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include <Arduino.h>

#define GRAVITE 9.80665 // metre / seconde caree

class Accelerometre
{

    uint8_t pinAcc;

public:
    uint16_t Zero;
    float Span;

    void init(uint8_t pinAnalog, uint16_t zero, float span)
    {
        pinAcc = pinAnalog;
        Zero = zero;
        Span = span;
    }

    void calibration()
    {
        Serial.println("Debut de la calibration Accelerometre");
        Serial.println("Mettre Axe accelerometre a 90 degree a la gravete pour Trouver le Zero");
        attUser("Mettre pin 13 au GND quand c'est Fait ", 20000);
        Zero = read();
        delay(500);
        Serial.println("Mettre Axe accelerometre a 0 degree a la gravete fleche vers le haut pour Trouver le Span de 9.81");
        attUser("Mettre pin 13 au GND quand c'est Fait ", 20000);
        Span = read() - Zero;
        afficherCalibration();
    }

    void afficherCalibration()
    {
        Serial.print("Zero : ");
        Serial.println(Zero);
        Serial.print("Span : ");
        Serial.println(Span);
    }

    uint16_t read()
    {
        return analogRead(pinAcc);
    }

    float convertRawToGForce(float rawAcc)
    {
        return (((float)rawAcc - (float)Zero) / Span) * GRAVITE; // En metre / seconde caree
    }

    void convertGForceToMparS2()
    {
    }

    void attUser(String info, uint32_t timeOut)
    {
        uint32_t timerOlder = millis();
        uint32_t timerOutOlder = millis();
        while (digitalRead(13))
        {
            //Tourne en boucle tant que la pin 13 est pas mis au ground
            if (millis() - timerOlder > 5000)
            {
                timerOlder = millis();
                Serial.println(info);
            }

            if (millis() - timerOutOlder > timeOut)
            {
                break;
            }
        }
    }
};