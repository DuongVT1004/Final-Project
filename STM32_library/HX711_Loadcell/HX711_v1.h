#ifndef _HX711_H_
#define _HX711_H_

#include "stm32f1xx_hal.h"

typedef struct
{
	uint32_t data;
	uint32_t data_ave;
	GPIO_TypeDef  *clk_gpio;
  GPIO_TypeDef  *data_gpio;
  uint16_t      clk_pin;
  uint16_t      data_pin;
}HX711_DataTypeDef;

void HX711_init(HX711_DataTypeDef*);
uint32_t HX711_Get_Value(HX711_DataTypeDef*);
uint32_t HX711_Ave(HX711_DataTypeDef*, uint16_t);

#endif