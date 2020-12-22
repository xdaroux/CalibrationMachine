#pragma once

#include <Arduino.h>

#define NB_RPM 5 //Nombre de rpm enregistre dans le tableau
#define RPM_MIN 200
#define MICROStoSECONDE 0.000001 // Multiplication pour transformer des uSec en S
#define SECONDEtoMINUTE 60
#define MICROStoMINUTE 0.00006
#define pinRPM 2
#define DELAY_ZERO_RPM 600000 // 0.6 secondes environ 100 Rpm

/*TYPEDEF STRUCT*/
typedef struct
{
	uint16_t rpm;

	volatile uint32_t timerOlder;
	volatile uint32_t timer;

	uint16_t rpmTableau[NB_RPM];
	uint8_t count;

} rpm_t;

void rpm_init(rpm_t *rpm);
void rpm_calcul(rpm_t *rpm);