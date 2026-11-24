#include "synth.h"

uint8_t sin_arr[SIN_SIZE];
volatile uint32_t scan_mask;
volatile Voice voices[VOICE_COUNT];
fn CurType;
GPIO_t pin_cols[4] = {{GPIOA, GPIO_PIN_4}, {GPIOA, GPIO_PIN_5}, {GPIOA, GPIO_PIN_6}, {GPIOA, GPIO_PIN_7}};
GPIO_t pin_rows[6] = {{GPIOB, GPIO_PIN_0}, {GPIOB, GPIO_PIN_1},
		              {GPIOB, GPIO_PIN_12}, {GPIOB, GPIO_PIN_13},
					  {GPIOB, GPIO_PIN_14}, {GPIOB, GPIO_PIN_15}};
FreqType_t freqs[24] = {261.63f, 277.18f, 293.66f, 311.13f, 329.63f, 349.23f,
		 	          369.99f, 392.00f, 415.30f, 440.00f, 466.16f, 493.88f,
					  523.25f, 554.37f, 587.33f, 622.25f, 659.26f, 698.46f,
					  739.99f, 783.99f, 830.61f, 880.00f, 932.22f, 987.77f};

uint8_t WaveSine(uint32_t phase) {return (uint8_t)sin_arr[phase >> 24];}
uint8_t WaveSquare(uint32_t phase) {return (uint8_t)((phase >> 24) > 127 ? 255 : 0);}

void SynthInit(void)
{
  for (int i = 0; i < 256; i++)
  {
    sin_arr[i] = (uint8_t)(128 + 127 * sinf(2.0f * M_PI * i / 256.0f));
  }
  for (int i = 0; i < VOICE_COUNT; i++)
  {
	voices[i].key = -1;
	voices[i].phase = 0;
	voices[i].angle = 0;
  }
  CurType = WaveSine;
}

uint32_t ScanKey(void)
{
	scan_mask = 0;
	for (int c = 0; c < 4; c++)
	{
		for (int i = 0; i < 4; i++)
		{
			HAL_GPIO_WritePin(pin_cols[i].port, pin_cols[i].pin, c == i ? GPIO_PIN_SET : GPIO_PIN_RESET);
		}
		for (int r = 0; r < 6; r++)
		{
			if (HAL_GPIO_ReadPin(pin_rows[r].port, pin_rows[r].pin) == GPIO_PIN_SET)
				scan_mask |= (1U << (6 * c + r));
		}
	}
	for (int i = 0; i < 4; i++)
	        HAL_GPIO_WritePin(pin_cols[i].port, pin_cols[i].pin, GPIO_PIN_RESET);
	return scan_mask;
}

uint32_t CalculateStep(const FreqType_t freq)
{
  return (uint32_t)(4294967296ULL * freq / 36000.0f);
}
/*
void CalculateDac(void)
{
  for (int i = 1; i < VOICE_COUNT; i++)
  {
	if (voice[i] != -1)
	{
	  voices[i].phase += voices[VOICE_COUNT - 1].angle;
      voices[i].dac_val = CurType(voices[VOICE_COUNT - 1].phase);
	}
  }
}
*/
void KeyPress(int8_t key)
{
  for (int i = 0; i < VOICE_COUNT; i++)
    if (voices[i].key == key) return;
  int idx = -1;
  for (int i = 0; i < VOICE_COUNT; i++)
	if (voices[i].key == -1) {idx = i; break;}
  if (idx < 0)
	idx = VOICE_COUNT - 1;
  voices[idx].angle = CalculateStep(freqs[key]);
  voices[idx].key = key;
}

void KeyUp(int8_t key)
{
  for (int i = 0; i < VOICE_COUNT; i++)
  {
    if (voices[i].key == key) {voices[i].key = -1; voices[i].angle = 0;}
  }
}


