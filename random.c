/*
 * random.c
 *
 *  Created on: 17 sty 2019
 *      Author: Dunajski
 */

#include <avr/interrupt.h>
#include <string.h>
#include "random.h"
#include "energy.h"

// temporary values.
volatile uint16 random_values_grouped[MAX_NUM_ACTIONS] = {5000, 5000, 5000, 5000, 5000};
volatile uchar change_random = 0;

/*
 *******************************************************************************
 * Obliczenie rejestru OCR1A dla Timer1, zaleznie od napiecia zasilania zeby 
 * uzyskac bezpieczne napiecie dla silnika ~3,3 V.
 * [in] adc_voltage - wartosc zmierzonego napiecia zasilania.
 *******************************************************************************
 */
static void SetOCRForProperVoltage(uint16 adc_voltage);

/*
 *******************************************************************************
 * Obsluga silnika tj. zalczania go oraz wylaczania zaleznie od losowych
 * wartosci wylosowanych z przetwornika ADC. Przerwanie co 10 ms. Dodatkowo do
 * wzbudzania silnika wymagany jest PWM.
 *******************************************************************************
 */
ISR(TIMER0_COMP_vect)
{
  // roznica wartosci w tablicy random_values o 1 stanowi roznice w czasie o 500 ms
  // stad wartosci w random values grouped beda sie roznic o 500 jednakze to oznacza
  // ISR wywolywane jest co 10 ms stad 500 ms / 10 ms oznacza 50 przerwan w celu
  // wyznaczenia 0,5 s znacznika czasu
  //  static uint8 time_divider = 0;
  static uint8 rnd_val_gr_idx = 0;
  static uint16 rnd_val_cnt = 0;

  if (device_state == ST_WIBROWANIE)
  {

    if (rnd_val_cnt >= (random_values_grouped[rnd_val_gr_idx] / 10))
    {
      rnd_val_cnt = 0;
      rnd_val_gr_idx++;
    }
    // indeksy parzyste oznaczaja wartosci w ktorych silnik jest uruchomiony
    // indeksy nieparzyste oznaczaja wartosci w ktorych silnik nie jest uruchomiony
    if (!(rnd_val_gr_idx % 2))
    {
      DEBUG_LED_ON;
      MOTOR_ON;
    }
    else
    {
      DEBUG_LED_OFF;
      MOTOR_OFF;
    }

    rnd_val_cnt++;

    if (rnd_val_gr_idx >= Seq_params.num_actions)
    {
      rnd_val_gr_idx = 0;
      device_state = ST_INTERAKCJA;
      #if DEBUG_STATE == _ON
      StrToSerial("Wylosowano i nadano, mozesz powtarzac\n");
      #endif
    }
  }

}

/*
 *******************************************************************************
 * Funkcja do wypelniania wylosowanymi wartosciami, tablice od nadawania
 * interakcji
 *******************************************************************************
 */
static void FillRandomValues(volatile uint16 random_values_grouped[MAX_NUM_ACTIONS],
                             volatile uchar random_values[NUM_RND])
{
  uint8 i;

  for (i = 0; i < MAX_NUM_ACTIONS; i++)
  {
    random_values_grouped[i] = random_values[2 * i] * 1500 + random_values[2 * i + 1] * 1500 + 1000;
  }

  // tymczasowe zmiany, stala sekwencja
  random_values_grouped[0] = 4000;
  random_values_grouped[1] = 1000;
  random_values_grouped[2] = 2000;
  random_values_grouped[3] = 1000;
  random_values_grouped[4] = 1000;

  memcpy((void *)Sequence.rnd_time, (void *)random_values_grouped, (sizeof(Sequence.rnd_time)));

//  #if DEBUG_STATE == _ON
//  for (int k = 0; k < MAX_NUM_ACTIONS; k++)
//  {
//    PutUInt16ToSerial(Sequence.rnd_time[k], 1, 5);
//    StrToSerial("\n");
//  }
//  #endif
}

volatile float wynik;
/*
 *******************************************************************************
 * Przerwanie odpowiedzialne za wylosowanie probek, zeby co interakcje dostac
 * inna sekwencje. Przetwonik jest wylaczany gdy nie jest potrzebny.
 *******************************************************************************
 */
ISR(ADC_vect)
{
  static uint8_t tmp_idx = 0;

  // sprawdzanie napiecia zasilania
  if (device_state == ST_MIERZENIE_ZASILANIA)
  {
    // dopiero drugi wynik jest wiarygodny
    if (!tmp_idx)
      tmp_idx++;

    // nastapila zmiana napiecia odniesienia oraz kanalu
    // wedlud pdfa dopiero 2gi wynik jest stabilny
    if (tmp_idx >= 1)
    {
      // wyzeruj index
      tmp_idx = 0;
      //oblicz wartosc OCRa zaleznie od napiecia zasilania
      SetOCRForProperVoltage(ADC);
      wynik = (2560.0 * ((float)ADC) )/ 1024.0;
      wynik /= 10;
      #if DEBUG_STATE == _ON
      StrToSerial("ADC = ");
      PutUInt16ToSerial(ADC, 1, 5);
      StrToSerial("\nVolt = ");
      PutUInt16ToSerial((uint16)wynik, 1, 4);
      StrToSerial("\n");
      #endif
      // przywroc stare ustawienia ADC
      InitAdc();
      // wylacz przetwornik
      TurnADCOff;
      // przejdz do wibrowania

      DelayandSetNextState(5, ST_WIBROWANIE);
    }
  }

  if (device_state == ST_LOSOWANIE)
  {
    // jesli losowanie, losuj tak dlugo az index bedzie rowny 13, tyle bitow losujemy
    if (change_random)
    {
      random_lsb = ADC & 0x01;
      random_values[tmp_idx] = random_lsb;
      tmp_idx++;
    }

    // jesli wylosowano 13 bitow przejd do wibrowania, wylacz ADC
    if (tmp_idx >= NUM_RND)
    {
      FillRandomValues(random_values_grouped, random_values);

      #if DEBUG_STATE == _ON
      StrToSerial("1: ");
      PutUInt16ToSerial(random_values_grouped[0], 1, 5);
      StrToSerial(" ms\n2: ");
      PutUInt16ToSerial(random_values_grouped[1], 1, 5);
      StrToSerial(" ms\n3: ");
      PutUInt16ToSerial(random_values_grouped[2], 1, 5);
      StrToSerial(" ms\n4: ");
      PutUInt16ToSerial(random_values_grouped[3], 1, 5);
      StrToSerial(" ms\n5: ");
      PutUInt16ToSerial(random_values_grouped[4], 1, 5);
      StrToSerial(" ms\nWylosowano probki, nadaje sekwencje\n");
      #endif

      tmp_idx = 0;
      change_random = 0;
      TurnADCOff;
      device_state = ST_MIERZENIE_ZASILANIA;
    }
  }

}

#define VCC_3V6_ADC_VAL 520UL // wartosc przetwornika dla napiecia 3V6, dzielnik napiecia i Vref 2,56V
/*
 *******************************************************************************
 * Obliczenie rejestru OCR1A dla Timer1, zaleznie od napiecia zasilania zeby 
 * uzyskac bezpieczne napiecie dla silnika ~3,3 V.
 * [in] adc_voltage - wartosc zmierzonego napiecia zasilania.
 *******************************************************************************
 */
static void SetOCRForProperVoltage(uint16 adc_voltage)
{
  uint32 OCR_set_val = 0x000000FF;
  // 0xFFFF na przetworniku daje 2,56 V
  // Dzielnik napiecia bedzie na zasilaniu, a wiec wszystko na pol czyli dla
  // 5V Vcc na pinie 2,5V <=> ADC = 1000
  // 3,4 Vcc na pinie 1,7V <=> ADC = 680
  // dla ponizszych moze byc juz 100% czyli jak najwyzsze napiecie i PWM jest 100%
  if (adc_voltage > VCC_3V6_ADC_VAL) // to jest 3,6V na VCC
    OCR_set_val = (VCC_3V6_ADC_VAL * 0x00FF) / adc_voltage;

  SetUint16_atomic(&OCR1A, (uint16)OCR_set_val);
}
