/*****************************************
* Project: BCM I4						 *
* Developer: Daniel Popa				 *
* Module: Analog to Digital Converter    *
* 		  Handler                        *
******************************************/
/*****************************************
*		INCLUDE PATHS					 *
******************************************/
#include "AdcH.h"
#include "App_Dependencies.h"
/*****************************************
*		END OF INCLUDE PATHS		     *
******************************************/
/*****************************************
*		VARIABLES					 	 *
******************************************/
/* Variable to store ADC state. */
uint8 Adc_BswState;
/* ADC buffer for channel one. */
uint32 Adc_ChannelOne_Buffer[ADC_BUFFER_LENGTH] = {STD_LOW};
/*****************************************
*		END OF VARIABLES				 *
******************************************/
/*****************************************
*		FUNCTIONS				 		 *
******************************************/
/* Peripheral initialization function declaration. */
StdReturnType Adc_Init();
/* Peripheral de-initialization function declaration. */
StdReturnType Adc_DeInit();
/* Peripheral main function declaration. */
VOID Adc_MainFunction();
/* Peripheral error callback function declaration. */
VOID HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc);
/* Peripheral conversion complete callback function declaration. */
VOID HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);
/*****************************************
*		END OF FUNCTIONS				 *
******************************************/
/***********************************************************************************
* Function: HAL_ADC_ConvCpltCallback										   	   *
* Description: Conversion callback to process conversion complete in the 		   *
* 			   peripheral.														   *
************************************************************************************/
VOID HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if(Os_Counter % 200 == 0)
	{
		do { SenCtrl_MeasuredValues[0] = Adc_ChannelOne_Buffer[0]; SenCtrl_MeasuredValues[1] = Adc_ChannelOne_Buffer[1]; } while (0);
	}
	else
	{
		/* do nothing */
	}
}
/***********************************************************************************
* END OF HAL_ADC_ConvCpltCallback										           *
************************************************************************************/
/***********************************************************************************
* Function: HAL_ADC_ErrorCallback										   		   *
* Description: Error callback to process fault in the peripheral.		 		   *
************************************************************************************/
VOID HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
	/* Local variable to store error status. */
	uint32 receivedStatus = HAL_ADC_GetError(&hadc1);
	/* Process error status.
	 * Perform re-initialization on each error to prevent faults in the peripheral. */
	switch(receivedStatus)
	{
		case HAL_ADC_ERROR_NONE:
			/* do nothing */
			break;
		case HAL_ADC_ERROR_INTERNAL:
			SystemManager_Fault[ADC_ERROR_INTERNAL]++;
			Adc_DeInit();
			Adc_Init();
			break;
		case HAL_ADC_ERROR_OVR:
			SystemManager_Fault[ADC_ERROR_OVR]++;
			Adc_DeInit();
			Adc_Init();
			break;
		case HAL_ADC_ERROR_DMA:
			SystemManager_Fault[ADC_ERROR_DMA]++;
			Adc_DeInit();
			Adc_Init();
			break;
		default:
			break;
	}
}
/***********************************************************************************
* END OF HAL_ADC_ErrorCallback										           	   *
************************************************************************************/
/***********************************************************************************
* Function: Adc_MainFunction										   			   *
* Description: Peripheral main function.									       *
************************************************************************************/
VOID Adc_MainFunction()
{
	/* Local variable to store the error status. */
	uint32 statusValue = STD_LOW;
	/* Get error status. */
	statusValue = HAL_ADC_GetState(&hadc1);

	/* Process module states. */
	switch(statusValue)
	{
		case HAL_ADC_STATE_RESET:
			Adc_BswState = statusValue;
			Adc_Init();
			break;
		case HAL_ADC_STATE_READY:
			Adc_BswState = statusValue;
			break;
		case HAL_ADC_STATE_BUSY:
			Adc_BswState = statusValue;
			break;
		case HAL_ADC_STATE_BUSY_REG:
			Adc_BswState = statusValue;
			break;
		case HAL_ADC_STATE_BUSY_INJ:
			Adc_BswState = statusValue;
			break;
		case HAL_ADC_STATE_TIMEOUT:
			Adc_BswState = statusValue;
			HAL_ADC_ErrorCallback(&hadc1);
			break;
		case HAL_ADC_STATE_ERROR:
			Adc_BswState = statusValue;
			HAL_ADC_ErrorCallback(&hadc1);
			break;
		default:
			break;
	}
}
/***********************************************************************************
* END OF Adc_MainFunction										           		   *
************************************************************************************/
/***********************************************************************************
* Function: Adc_DeInit										   					   *
* Description: De-initialize the peripheral.									   *
************************************************************************************/
StdReturnType Adc_DeInit()
{
	/* Call for de-intialization function. */
	HAL_ADC_DeInit(&hadc1);
	return E_OK;
}
/***********************************************************************************
* END OF Adc_DeInit										           				   *
************************************************************************************/
/***********************************************************************************
* Function: Adc_Init										   					   *
* Description: Initialize the protocol.									       	   *
************************************************************************************/
StdReturnType Adc_Init()
{
	ADC_AnalogWDGConfTypeDef AnalogWDGConfig = {0};
	ADC_ChannelConfTypeDef sConfig = {0};
	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.ScanConvMode = ENABLE;
	hadc1.Init.ContinuousConvMode = ENABLE;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = 3;
	hadc1.Init.DMAContinuousRequests = ENABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	if (HAL_ADC_Init(&hadc1) != HAL_OK)
	{
		HAL_ADC_ErrorCallback(&hadc1);
	}
	else
	{
		/* do nothing */
	}
	AnalogWDGConfig.WatchdogMode = ADC_ANALOGWATCHDOG_SINGLE_REG;
	AnalogWDGConfig.HighThreshold = 3000;
	AnalogWDGConfig.LowThreshold = 0;
	AnalogWDGConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
	AnalogWDGConfig.ITMode = ENABLE;
	if (HAL_ADC_AnalogWDGConfig(&hadc1, &AnalogWDGConfig) != HAL_OK)
	{
		HAL_ADC_ErrorCallback(&hadc1);
	}
	else
	{
		/* do nothing */
	}
	sConfig.Channel = ADC_CHANNEL_13;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		HAL_ADC_ErrorCallback(&hadc1);
	}
	else
	{
		/* do nothing */
	}
	sConfig.Channel = ADC_CHANNEL_12;
	sConfig.Rank = 2;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		HAL_ADC_ErrorCallback(&hadc1);
	}
	else
	{
		/* do nothing */
	}
	sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
	sConfig.Rank = 3;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		HAL_ADC_ErrorCallback(&hadc1);
	}
	else
	{
		/* do nothing */
	}
	HAL_ADC_Start_DMA(&hadc1, Adc_ChannelOne_Buffer, ADC_BUFFER_LENGTH);
	return E_OK;
}
/***********************************************************************************
* END OF Adc_Init										           				   *
************************************************************************************/
