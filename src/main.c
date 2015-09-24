
#include "main.h"
#include "usbd_hid_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"

#define ADC1_CDR_Address    ((uint32_t)0x40012308)



#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ )
    #pragma data_alignment = 4   
  #endif
#endif
__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;
  
uint32_t ADC_ConvertedValueX = 0;
uint32_t ADC_ConvertedValueX_1 = 0;
__IO uint8_t UserButtonPressed = 0x00;
uint8_t InBuffer[64], OutBuffer[63];

void Init_ADC_Reading(void);

int main(void)
{
  STM32F4_Discovery_LEDInit(LED3);
  STM32F4_Discovery_LEDInit(LED4);
  STM32F4_Discovery_LEDInit(LED5);
  STM32F4_Discovery_LEDInit(LED6);
  STM32F4_Discovery_PBInit(BUTTON_USER, BUTTON_MODE_GPIO);
  STM32F4_Discovery_LEDOn(LED3);
  Delay(0xFFFF);
  STM32F4_Discovery_LEDOff(LED3);

  USBD_Init(&USB_OTG_dev,
#ifdef USE_USB_OTG_HS
  USB_OTG_HS_CORE_ID,
#else
  USB_OTG_FS_CORE_ID,
#endif
  &USR_desc,
  &USBD_HID_cb,
  &USR_cb);


  Init_ADC_Reading();
  int x=0;
  while (1)
  {
	  Delay(0xFF);
	  if (OutBuffer[1]%2==1)
	  {
		  switch(x)
		  {
		  case 1:
			  STM32F4_Discovery_LEDOn(LED3);
			  break;
		  case 999999:
			  STM32F4_Discovery_LEDOff(LED3);
			  STM32F4_Discovery_LEDOn(LED4);
			  break;
		  case 2*999999:
			  STM32F4_Discovery_LEDOff(LED4);
			  STM32F4_Discovery_LEDOn(LED5);
			  break;
		  case 3*999999:
			  STM32F4_Discovery_LEDOff(LED5);
			  STM32F4_Discovery_LEDOn(LED6);
			  break;
		  case 4*999999:
			  STM32F4_Discovery_LEDOff(LED6);
		       x=0;
		  }
		  x++;
	  }
	  else
	  {
		  if (OutBuffer[0]%5==1)  	STM32F4_Discovery_LEDOn(LED3);
			  	  else			STM32F4_Discovery_LEDOff(LED3);
		  if (OutBuffer[0]%5==2)  	STM32F4_Discovery_LEDOn(LED4);
			  	  else			STM32F4_Discovery_LEDOff(LED4);
		  if (OutBuffer[0]%5==3)  	STM32F4_Discovery_LEDOn(LED5);
			  	  else			STM32F4_Discovery_LEDOff(LED5);
		  if (OutBuffer[0]%5==4)  	STM32F4_Discovery_LEDOn(LED6);
			  	  else			STM32F4_Discovery_LEDOff(LED6);
	  }

  }
}


void Init_ADC_Reading(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure;

	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE);

	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);

	  ADC_InitTypeDef ADC_InitStructure;
	  ADC_CommonInitTypeDef ADC_CommonInitStructure;
	  DMA_InitTypeDef DMA_InitStructure;
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

		ADC_DeInit();

		DMA_DeInit( DMA1_Stream0);


		DMA_InitStructure.DMA_Channel = DMA_Channel_1;
		DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_CDR_Address; //Address of peripheral the DMA must map to
		DMA_InitStructure.DMA_Memory0BaseAddr  = (uint32_t)&ADC_ConvertedValueX;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
		DMA_InitStructure.DMA_BufferSize = 1;
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
		DMA_InitStructure.DMA_Priority = DMA_Priority_High;
		DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
		DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
		DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
		DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

		DMA_Init(DMA1_Stream0, &DMA_InitStructure);

		DMA_ITConfig(DMA1_Stream0, DMA_IT_TCIF0, ENABLE);

		ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
		ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
		ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
		ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
		ADC_CommonInit(&ADC_CommonInitStructure);

		ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
		ADC_InitStructure.ADC_ScanConvMode        = ENABLE;
		ADC_InitStructure.ADC_ContinuousConvMode  = ENABLE;
		ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
		ADC_InitStructure.ADC_DataAlign           = ADC_DataAlign_Right;
		ADC_InitStructure.ADC_NbrOfConversion = 1;
		ADC_Init( ADC1, &ADC_InitStructure );

		ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_3Cycles);

		ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
		ADC_DMACmd(ADC1, ENABLE);

		ADC_Cmd(ADC1, ENABLE);


		ADC_SoftwareStartConv(ADC1);

	  NVIC_InitTypeDef NVIC_InitStructure;

	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream0_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_Init(&NVIC_InitStructure);
}

void Delay(__IO uint32_t nTime)
{
  if (nTime != 0x00)
  { 
    nTime--;
  }
}


#ifdef  USE_FULL_ASSERT


void assert_failed(uint8_t* file, uint32_t line)
{ 
  while (1)
  {
  }
}
#endif

