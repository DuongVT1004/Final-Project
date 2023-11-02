#include "dht11.h"

uint32_t DWT_Delay_Init(void)
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

void Set_Pin_Output(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void Set_Pin_Input(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void MCU_Send_Start()
{
	DWT_Delay_Init();
	Set_Pin_Output(DHT_PORT, DHT_PIN);
	HAL_GPIO_WritePin(DHT_PORT, DHT_PIN, 0);
	delay(18000);  // Wait for 18 ms
	HAL_GPIO_WritePin(DHT_PORT, DHT_PIN, 1);
	delay(20);  // Wait for 20us
	Set_Pin_Input(DHT_PORT, DHT_PIN);
}

uint8_t DHT_Response()
{
	uint8_t Response = 0;
	delay(40);
	if(!(HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN)))
	{
		delay(80);
		if(HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN)) 
		{
			Response = 1;
		}
		else{
			Response = 0;
		}
	}
	while(HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN));
	return Response;
}

uint8_t DHT_Read()
{
	uint8_t i = 0, j;
	for(j = 0; j < 8; j++)
	{
		while(!HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN));
		delay(40);
		if(!HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN))
		{
			i &= ~(1 << (7 - j)); // Write bit 0
		}
		else i |= (1 << (7 - j)); // Write bit 1
		while(HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN));
	}
	return i;
}

void DHT_Get_Data(DHT_DataTypeDef *DHT_Data)
{
	uint8_t Hum_Byte1, Hum_Byte2, Tem_Byte1, Tem_Byte2, Check_Sum;
	MCU_Send_Start();
	uint8_t CheckRP = DHT_Response();
	Hum_Byte1 = DHT_Read();
	Hum_Byte2 = DHT_Read();
	Tem_Byte1 = DHT_Read();
	Tem_Byte2 = DHT_Read();
	Check_Sum = DHT_Read();
	if(Check_Sum == (Hum_Byte1 + Hum_Byte2 + Tem_Byte1 + Tem_Byte2))
	{
		DHT_Data -> Humidity = Hum_Byte1;
		DHT_Data -> Temperature = Tem_Byte1;
	}
}




