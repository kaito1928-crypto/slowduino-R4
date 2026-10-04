/**
 * @file hal_ra4m1.h
 * @brief Hardware layer do Arduino UNO R4 (Renesas RA4M1).
 *
 * O núcleo do motor continua o do 328P. Aqui ficam só os periféricos:
 *
 *   Trigger     D2 / P105, attachInterrupt + micros() do core (AGT0).
 *   Ignição     GPT0, free-running, compare A/B, tick lógico de 16 µs.
 *   Injeção     o polling em micros() que o 328P já usa (sem timer extra).
 *   IAC         AGT1, tick de 252 µs (~3968 Hz), PWM por software em D9.
 *   ADC         analogRead 10 bits. A0-A5 no header. A6/A7 só se o
 *               variant do core os definir.
 *   EEPROM      data flash do core (EEPROM.h), não RAM.
 *   Serial      USB CDC (o core faz #define Serial SerialUSB).
 *
 * AGT0 não é usado: o core reserva esse timer para millis/micros.
 */

#ifndef HAL_RA4M1_H
#define HAL_RA4M1_H

#include <Arduino.h>

#if defined(BOARD_RA4M1)

bool ra4m1IgnitionBegin(void (*compareA)(), void (*compareB)());
bool ra4m1IgnitionReady();

bool ra4m1IacBegin(void (*tick)());
void ra4m1IacStart();
void ra4m1IacStop();
bool ra4m1IacRunning();

#endif

#endif // HAL_RA4M1_H
