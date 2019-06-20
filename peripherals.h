/*
 * peripherals.h
 *
 *  Created on: 17 sty 2019
 *      Author: Dunajski
 */

#ifndef PERIPHERALS_H_
#define PERIPHERALS_H_

#include "communication.h"
#include "random.h"
#include "types.h"

#define ISR_DEBOUNCE_CNT 200

typedef struct PortABits
{
  volatile uchar adc_pin :1;    // PA 0
  volatile uchar :1;            // PA 1
  volatile uchar :3;            // PA 2-4
  volatile uchar motor :1;      // PA 5
  volatile uchar debug_led :1;  // PA 6 // do prototypowania nie beda lutowane
  volatile uchar state_led :1;  // PA 7 // do prototypowania nie beda lutowane
} TPortABits;

typedef struct PortDBits
{
  volatile uchar :2;            // PD 0-1
  volatile uchar action_key:1;  // PD 2
  volatile uchar :5;            // PA 3-7
} TPortDBits;

#define STATE_LED_VAL ((TPortABits *)&PINA)->state_led
#define STATE_LED_DIR ((TPortABits *)&DDRA)->state_led
#define STATE_LED_OUT ((TPortABits *)&PORTA)->state_led

#define DEBUG_LED_VAL ((TPortABits *)&PINA)->debug_led
#define DEBUG_LED_DIR ((TPortABits *)&DDRA)->debug_led
#define DEBUG_LED_OUT ((TPortABits *)&PORTA)->debug_led

#define MOTOR_VAL ((TPortABits *)&PINA)->motor
#define MOTOR_DIR ((TPortABits *)&DDRA)->motor
#define MOTOR_OUT ((TPortABits *)&PORTA)->motor

#define ACTION_KEY_VAL    ((TPortDBits *)&PIND)->action_key
#define ACTION_KEY_DIR    ((TPortDBits *)&DDRD)->action_key
#define ACTION_KEY_PULLUP ((TPortDBits *)&PORTD)->action_key

#define ADC_PIN_VAL     ((TPortABits *)&PINA)->adc_pin
#define ADC_PIN_DIR     ((TPortABits *)&DDRA)->adc_pin
#define ADC_PIN_PULLUP  ((TPortABits *)&PORTA)->adc_pin

#define DEBUG_LED_ON   (DEBUG_LED_OUT=1)
#define DEBUG_LED_OFF  (DEBUG_LED_OUT=0)
#define DEBUG_LED_TOGGLE  (DEBUG_LED_OUT=(!DEBUG_LED_OUT))

#define STATE_LED_ON   (STATE_LED_OUT=1)
#define STATE_LED_OFF  (STATE_LED_OUT=0)
#define STATE_LED_TOGGLE  (STATE_LED_OUT=(!STATE_LED_OUT))

#define MOTOR_ON   (MOTOR_OUT=1)
#define MOTOR_OFF  (MOTOR_OUT=0)

/*
 *******************************************************************************
 * Inicjuje UART, 9600/8N1. Komunikacja na przerwaniach.
 *******************************************************************************
 */
void InitUsart(void);

/*
 *******************************************************************************
 * Inicjalizacja Timer0 do sterowania silnikiem, 10 ms CTC/presc. 1024.
 *******************************************************************************
 */
void InitTimer0(void);

/*
 *******************************************************************************
 * Inicjalizacja Timer2 do obslugi stanow oraz przycisku, 0,2 ms CTC/presc. 8.
 *******************************************************************************
 */
void InitTimer2(void);

/*
 *******************************************************************************
 * Inicjalizacja przetwornika do losowania zmiennych. ADC0, prescaler 64,
 * odniesienia na AVCC.
 *******************************************************************************
 */
void InitAdc(void);

/*
 *******************************************************************************
 * Inicjalizacja wejsc/wyjsc MCU. Tutaj ustawiam LEDy i stan poczatkowy appki.
 *******************************************************************************
 */
void InitIO(void);

/*
 *******************************************************************************
 * Przeprowadza procedure usypiania MCU, wylaczajac przerwania od wszystkich
 * peryferiow oprocz przerwania zewnetrznego. Nastepnie po przebudzeniu
 * przywraca wszystkie przerwania od peryferiow.
 *******************************************************************************
 */
void GoToSleep(void);

/*
 *******************************************************************************
 * Inicjalizacja External Interrupt 0 do wybudzania MCU. Stan niski na PD2.
 *******************************************************************************
 */
void InitExternalInterupt1(void);

#endif /* PERIPHERALS_H_ */
