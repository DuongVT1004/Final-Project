#ifndef _DHT_H_
#define _DHT_H_

#include "stm32f1xx_hal.h"

#define DHT_PORT GPIOA
#define DHT_PIN GPIO_PIN_6

typedef struct{
	uint8_t Humidity;
	uint8_t Temperature;
}DHT_DataTypeDef;

void DHT_Get_Data(DHT_DataTypeDef *DHT_Data);

#endif