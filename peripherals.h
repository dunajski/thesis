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
  volatile uchar :3;            // PD 0-2
  volatile uchar action_key:1;  // PD 3
  volatile uchar :4;            // PA 4-7
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

#define ACTION_KEY_VAL    ((TPortDBits *)&PINA)->action_key
#define ACTION_KEY_DIR    ((TPortDBits *)&DDRA)->action_key
#define ACTION_KEY_PULLUP ((TPortDBits *)&PORTA)->action_key

#define ADC_PIN_VAL     ((TPortABits *)&PINA)->adc_pin
#define ADC_PIN_DIR     ((TPortABits *)&DDRA)->adc_pin
#define ADC_PIN_PULLUP  ((TPortABits *)&PORTA)->adc_pin

#define DEBUG_LED_ON   (DEBUG_LED_OUT=1)
#define DEBUG_LED_OFF  (DEBUG_LED_OUT=0)

#define MOTOR_ON   (MOTOR_OUT=1)
#define MOTOR_OFF  (MOTOR_OUT=0)

void InitUart(void);
void InitTimer0(void);
void InitTimer2(void);
void InitAdc(void);
void InitIO(void);
void GoToSleep(void);

#endif /* PERIPHERALS_H_ */
