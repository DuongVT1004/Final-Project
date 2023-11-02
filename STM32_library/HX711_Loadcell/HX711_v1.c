#include "HX711_v1.h"

static uint32_t DWT_Delay_Init(void)
{
  /* Disable TRC */
  CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk; // ~0x01000000;
  /* Enable TRC */
  CoreDebug->DEMCR |=  CoreDebug_DEMCR_TRCENA_Msk; // 0x01000000;

  /* Disable clock cycle counter */
  DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; //~0x00000001;
  /* Enable  clock cycle counter */
  DWT->CTRL |=  DWT_CTRL_CYCCNTENA_Msk; //0x00000001;

  /* Reset the clock cycle counter value */
  DWT->CYCCNT = 0;

     /* 3 NO OPERATION instructions */
     __ASM volatile ("NOP");
     __ASM volatile ("NOP");
     __ASM volatile ("NOP");

  /* Check if clock cycle counter has started */
     if(DWT->CYCCNT)
     {
       return 0; /*clock cycle counter started*/
     }
     else
  {
    return 1; /*clock cycle counter not started*/
  }
}

__STATIC_INLINE void delay(volatile uint32_t au32_microseconds)
{
  uint32_t au32_initial_ticks = DWT->CYCCNT;
  uint32_t au32_ticks = (HAL_RCC_GetHCLKFreq() / 1000000);
  au32_microseconds *= au32_ticks;
  while ((DWT->CYCCNT - au32_initial_ticks) < au32_microseconds-au32_ticks);
}

//void delay(void)
//{
//  uint32_t delay = 4;
//  while (delay > 0)
//  {
//    delay--;
//    __nop(); __nop(); __nop(); __nop();    
//  }
//}

void Set_Pin_Clk(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void Set_Pin_Data(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void HX711_init(HX711_DataTypeDef *hx711)
{
	Set_Pin_Clk(hx711->clk_gpio, hx711->clk_pin);
	Set_Pin_Data(hx711->data_gpio, hx711->data_pin);
	DWT_Delay_Init();
}

uint32_t HX711_Get_Value(HX711_DataTypeDef *hx711)
{
	int32_t data = 0;
	uint8_t i;
	uint32_t startTime = HAL_GetTick();
	HAL_GPIO_WritePin(hx711->clk_gpio, hx711->clk_pin, GPIO_PIN_RESET);
	while(HAL_GPIO_ReadPin(hx711->data_gpio, hx711->data_pin) == GPIO_PIN_SET);
//	{
//		HAL_Delay(1);
//		if(HAL_GetTick() - startTime > 150)
//		{ 
//	       return 0;
//	    }
//	}
	for(i = 0; i < 24; i++)
	{
		HAL_GPIO_WritePin(hx711->clk_gpio, hx711->clk_pin, GPIO_PIN_SET);
		delay(30);
		HAL_GPIO_WritePin(hx711->clk_gpio, hx711->clk_pin, GPIO_PIN_RESET);
		delay(30);
		data = data << 1;
		if(HAL_GPIO_ReadPin(hx711->data_gpio, hx711->data_pin) == GPIO_PIN_SET)
		{
			data++;
		}
	}
	data = data ^ 0x800000;
	HAL_GPIO_WritePin(hx711->clk_gpio, hx711->clk_pin, GPIO_PIN_SET);
	delay(30);
	HAL_GPIO_WritePin(hx711->clk_gpio, hx711->clk_pin, GPIO_PIN_RESET);
	delay(30);
	return data;
}

uint32_t HX711_Ave(HX711_DataTypeDef *hx711, uint16_t sample)
{
	uint64_t ave = 0;
	uint8_t i;
	for(i = 0; i < sample; i++)
	{
		ave += HX711_Get_Value(hx711);
		HAL_Delay(5);
	}
	uint32_t final_data = (uint32_t)(ave/sample);
	return final_data;
}