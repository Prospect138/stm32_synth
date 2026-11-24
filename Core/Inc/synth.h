#ifndef __SYNTH_H
#define __SYNTH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include <math.h>
#include <stdint.h>

#define SIN_SIZE 256
#define VOICE_COUNT 4

// -----typedefs----
typedef uint8_t (*fn)(uint32_t phase);
typedef float FreqType_t;
typedef struct
{
	GPIO_TypeDef* port;
	uint16_t pin;
} GPIO_t;
typedef struct
{
  uint32_t angle;
  uint32_t phase;
  int8_t key;
  //uint32_t dac_val;
} Voice;

// -----DATA-----
extern uint8_t sin_arr[SIN_SIZE];
extern volatile uint32_t scan_mask;
extern volatile Voice voices[VOICE_COUNT];
//extern volatile uint32_t dac_value;
extern fn CurType;
extern GPIO_t pin_cols[4];
extern GPIO_t pin_rows[6];
extern FreqType_t freqs[24];

// ------FUNCTIONS-----
uint8_t WaveSine(uint32_t phase);
uint8_t WaveSquare(uint32_t phase);
void SynthInit(void);
uint32_t ScanKey(void);
uint32_t CalculateStep(const FreqType_t freq);
//void CalculateDac(void);
void KeyPress(int8_t key);
void KeyUp(int8_t key);


#ifdef __cplusplus
}
#endif // extern "C"

#endif // __SYNTH_H
