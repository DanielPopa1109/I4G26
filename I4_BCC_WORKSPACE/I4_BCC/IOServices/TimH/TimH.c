/*****************************************
* Project: BCM I4						 *
* Developer: Daniel Popa				 *
* Module: Timer Handler		 			 *
******************************************/
/*****************************************
*		INCLUDE PATHS					 *
******************************************/
#include "TimH.h"
#include "PortH.h"
#include "App_Dependencies.h"
/*****************************************
*		END OF INCLUDE PATHS		     *
******************************************/
/*****************************************
*		VARIABLES					 	 *
******************************************/
/* Variable to store module state. */
uint8 TimH_BswState_TimerTwo = STD_LOW;
/* Variable to store module state. */
uint8 TimH_BswState_TimerThree = STD_LOW;
/* Variable to store module state. */
uint8 TimH_BswState_TimerFour = STD_LOW;
/* Variable to store module state. */
uint8 TimH_BswState_TimerFive = STD_LOW;
/* First time stamp of the input compare value for channel three. */
uint32 Tim5_InputCompare_ValueOne_ChannelThree = 0;
/* Second time stamp of the input compare value for channel three. */
uint32 Tim5_InputCompare_ValueTwo_ChannelThree = 0;
/* Variable for the difference between the two time stamps for channel three. */
uint32 Tim5_Difference_ChannelThree = 0;
/* Input capture flag variable for channel three. */
uint8 Tim5_InputCaptureFlag_ChannelThree = 0;
/* Distance calculated for the distance traveled by the sound waves between the two input captures time stamps for channel three. */
uint8 Tim5_CalculatedDistance_ChannelThree = 0;
/* First time stamp of the input compare value for channel four. */
uint32 Tim5_InputCompare_ValueOne_ChannelFour = 0;
/* Second time stamp of the input compare value for channel four. */
uint32 Tim5_InputCompare_ValueTwo_ChannelFour = 0;
/* Variable for the difference between the two time stamps for channel four. */
uint32 Tim5_Difference_ChannelFour = 0;
/* Input capture flag variable for channel four. */
uint8 Tim5_InputCaptureFlag_ChannelFour = 0;
/* Distance calculated for the distance traveled by the sound waves between the two input captures time stamps for channel four. */
uint8 Tim5_CalculatedDistance_ChannelFour = 0;
/* Variable to store first call at start-up. */
STATIC uint8 TimH_MainFunctionFirstCall = 0;
/* Static variable used in timer configuration. */
STATIC TIM_ClockConfigTypeDef sClockSourceConfig2 = {0};
/* Static variable used in timer configuration. */
STATIC TIM_MasterConfigTypeDef sMasterConfig2 = {0};
/* Static variable used in timer configuration. */
STATIC TIM_OC_InitTypeDef sConfigOC2 = {0};
/* Static variable used in timer configuration. */
STATIC TIM_OC_InitTypeDef sConfigOC3 = {0};
/* Static variable used in timer configuration. */
STATIC TIM_OC_InitTypeDef sConfigOC4 = {0};
/* Static variable used in timer configuration. */
STATIC TIM_ClockConfigTypeDef sClockSourceConfig3 = {0};
/* Static variable used in timer configuration. */
STATIC TIM_MasterConfigTypeDef sMasterConfig3 = {0};
/* Static variable used in timer configuration. */
STATIC TIM_ClockConfigTypeDef sClockSourceConfig4 = {0};
/* Static variable used in timer configuration. */
STATIC TIM_MasterConfigTypeDef sMasterConfig4 = {0};
/* Static variable used in timer configuration. */
STATIC TIM_ClockConfigTypeDef sClockSourceConfig5 = {0};
/* Static variable used in timer configuration. */
STATIC TIM_MasterConfigTypeDef sMasterConfig5 = {0};
/* Static variable used in timer configuration. */
STATIC TIM_IC_InitTypeDef sConfigIC5 = {0};
/*****************************************
*		END OF VARIABLES				 *
******************************************/
/*****************************************
*		FUNCTIONS				 		 *
******************************************/
/* Peripheral initialization function declaration. */
StdReturnType Tim_Init(uint8 TimerChannel);
/* Peripheral de-initialization function declaration. */
StdReturnType Tim_DeInit(uint8 TimerChannel);
/* Peripheral error callback function declaration. */
VOID HAL_TIM_ErrorCallback(TIM_HandleTypeDef *htim);
/* Peripheral period elapsed callback function declaration. */
VOID HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
/* Peripheral input capture callback function declaration. */
VOID HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
/* Peripheral main function declaration. */
VOID Tim_MainFunction();
/*****************************************
*		END OF FUNCTIONS				 *
******************************************/
/***********************************************************************************
* Function: HAL_TIM_IC_CaptureCallback										   	   *
* Description: Input capture callback to process information received.		 	   *
************************************************************************************/
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	/* If channel 3 requests. */
	if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
	{
		/* Check if the first input capture has been received. */
		if (Tim5_InputCaptureFlag_ChannelThree == STD_LOW)
		{
			/* Store the input capture value. */
			Tim5_InputCompare_ValueOne_ChannelThree = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			/* Set the flag to high. */
			Tim5_InputCaptureFlag_ChannelThree = STD_HIGH;
			/* Change the polarity of the input channel. */
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
		}/* If the first input capture has been stored. */
		else if(Tim5_InputCaptureFlag_ChannelThree == STD_HIGH)
		{
			/* Store the second input capture. */
			Tim5_InputCompare_ValueTwo_ChannelThree = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			/* Set the timer counter to 0. */
			__HAL_TIM_SET_COUNTER(htim, 0);
			/* If the second input capture is greater than the first, then subtract the first one from the second one. */
			if (Tim5_InputCompare_ValueTwo_ChannelThree > Tim5_InputCompare_ValueOne_ChannelThree)
			{
				Tim5_Difference_ChannelThree = Tim5_InputCompare_ValueTwo_ChannelThree - Tim5_InputCompare_ValueOne_ChannelThree;
			}/* Else subtract the first value from maximum uint32 value and add the second value. */
			else if(Tim5_InputCompare_ValueOne_ChannelThree > Tim5_InputCompare_ValueTwo_ChannelThree)
			{
				Tim5_Difference_ChannelThree = (0xFFFFFFFF - Tim5_InputCompare_ValueOne_ChannelThree) + Tim5_InputCompare_ValueTwo_ChannelThree;
			}
			else
			{
				/* do nothing */
			}
			/* Calculate the distance the sound traveled between the first measurement and the second. */
			Tim5_CalculatedDistance_ChannelThree = Tim5_Difference_ChannelThree * 0.034 / 2;
			/* Set the flag to low. */
			Tim5_InputCaptureFlag_ChannelThree = STD_LOW;
			/* Change the polarity of the input channel. */
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
			/* Disable the interrupt for the timer 5 channel 3. */
			__HAL_TIM_DISABLE_IT(&htim5, TIM_IT_CC1);
		}
		else
		{
			/* do nothing */
		}
	}/* If channel 4 requests. */

	if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
	{
		/* Check if the first input capture has been received. */
		if (Tim5_InputCaptureFlag_ChannelFour == STD_LOW)
		{
			/* Store the input capture value. */
			Tim5_InputCompare_ValueOne_ChannelFour = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
			/* Set the flag to high. */
			Tim5_InputCaptureFlag_ChannelFour = STD_HIGH;
			/* Change the polarity of the input channel. */
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_FALLING);
		}/* If the first input capture has been stored. */
		else if(Tim5_InputCaptureFlag_ChannelFour == STD_HIGH)
		{
			/* Store the second input capture. */
			Tim5_InputCompare_ValueTwo_ChannelFour = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
			/* Set the timer counter to 0. */
			__HAL_TIM_SET_COUNTER(htim, 0);
			/* If the second input capture is greater than the first, then subtract the first one from the second one. */
			if (Tim5_InputCompare_ValueTwo_ChannelFour > Tim5_InputCompare_ValueOne_ChannelFour)
			{
				Tim5_Difference_ChannelFour = Tim5_InputCompare_ValueTwo_ChannelFour - Tim5_InputCompare_ValueOne_ChannelFour;
			}/* Else subtract the first value from maximum uint32 value and add the second value. */
			else if(Tim5_InputCompare_ValueOne_ChannelFour > Tim5_InputCompare_ValueTwo_ChannelFour)
			{
				Tim5_Difference_ChannelFour = (0xFFFFFFFF - Tim5_InputCompare_ValueOne_ChannelFour) + Tim5_InputCompare_ValueTwo_ChannelFour;
			}
			else
			{
				/* do nothing */
			}
			/* Calculate the distance the sound traveled between the first measurement and the second. */
			Tim5_CalculatedDistance_ChannelFour = Tim5_Difference_ChannelFour * 0.034 / 2;
			/* Set the flag to low. */
			Tim5_InputCaptureFlag_ChannelFour = 0;
			/* Change the polarity of the input channel. */
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_RISING);
			/* Disable the interrupt for the timer 5 channel 4. */
			__HAL_TIM_DISABLE_IT(&htim5, TIM_IT_CC2);
		}
		else
		{
			/* do nothing */
		}
	}
	else
	{
		/* do nothing */
	}
}
/***********************************************************************************
* END OF HAL_TIM_IC_CaptureCallback										           *
************************************************************************************/
/***********************************************************************************
* Function: Tim_Init										   			           *
* Description: Peripheral initialization.									       *
************************************************************************************/
StdReturnType Tim_Init(uint8 TimerChannel)
{
	/* Set the timer channel value into a local variable. */
	uint8 localValue = TimerChannel;
	switch(localValue)
	{
		/* Initialize the timer two. */
		case TIMER_TWO:
			/* Configure the timer parameters. */
			htim2.Instance = TIM2;
			htim2.Init.Prescaler = 1000-1;
			htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
			htim2.Init.Period = 2000-1;
			htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
			htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
			/* If initialization is inadequate, call for the error callback. */
			if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
			{
				HAL_TIM_ErrorCallback(&htim2);
			}
			else
			{
				/* do nothing */
			}
			/* Set the timer clock source. */
			sClockSourceConfig2.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
			/* If initialization is inadequate, call for the error callback. */
			if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig2) != HAL_OK)
			{
				HAL_TIM_ErrorCallback(&htim2);
			}
			else
			{
				/* do nothing */
			}
			/* If initialization is inadequate, call for the error callback. */
			if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
			{
				HAL_TIM_ErrorCallback(&htim2);
			}
			else
			{
				/* do nothing */
			}
			/* Set the master slave mode and output trigger. */
			sMasterConfig2.MasterOutputTrigger = TIM_TRGO_RESET;
			sMasterConfig2.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
			/* If initialization is inadequate, call for the error callback. */
			if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig2) != HAL_OK)
			{
				HAL_TIM_ErrorCallback(&htim2);
			}
			else
			{
				/* do nothing */
			}
			/* Set the PWM parameters. */
			sConfigOC2.OCMode = TIM_OCMODE_PWM1;
			sConfigOC2.Pulse = 0;
			sConfigOC2.OCPolarity = TIM_OCPOLARITY_HIGH;
			sConfigOC2.OCFastMode = TIM_OCFAST_ENABLE;
			/* If initialization is inadequate, call for the error callback. */
			if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC2, TIM_CHANNEL_1) != HAL_OK)
			{
				HAL_TIM_ErrorCallback(&htim2);
			}
			else
			{
				/* do nothing */
			}
			/* If initialization is inadequate, call for the error callback. */
			if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC2, TIM_CHANNEL_2) != HAL_OK)
			{
				HAL_TIM_ErrorCallback(&htim2);
			}
			else
			{
				/* do nothing */
			}
			/* If initialization is inadequate, call for the error callback. */
			if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC2, TIM_CHANNEL_3) != HAL_OK)
			{
				HAL_TIM_ErrorCallback(&htim2);
			}
			else
			{
				/* do nothing */
			}
			/* Configure the GPIO. */
			HAL_TIM_MspPostInit(&htim2);

			break;
		/* Initialize the timer three. */
		case TIMER_THREE:
			/* Configure the timer parameters. */
			htim3.Instance = TIM3;
			htim3.Init.Prescaler = 1000-1;
			htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
			htim3.Init.Period = 2000-1;
			htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
			htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
			/* If initialization is inadequate, call for the error callback. */
			if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
			{
			  HAL_TIM_ErrorCallback(&htim3);
			}
			else
			{
			  /* do nothing */
			}
			/* Set the clock source. */
			sClockSourceConfig3.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
			/* If initialization is inadequate, call for the error callback. */
			if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig3) != HAL_OK)
			{
			  HAL_TIM_ErrorCallback(&htim3);
			}
			else
			{
			  /* do nothing */
			}
			/* If initialization is inadequate, call for the error callback. */
			if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
			{
			  HAL_TIM_ErrorCallback(&htim3);
			}
			else
			{
			  /* do nothing */
			}
			/* Configure master slave mode. */
			sMasterConfig3.MasterOutputTrigger = TIM_TRGO_RESET;
			sMasterConfig3.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
			/* If initialization is inadequate, call for the error callback. */
			if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig3) != HAL_OK)
			{
			  HAL_TIM_ErrorCallback(&htim3);
			}
			else
			{
			  /* do nothing */
			}
			/* Configure PWM parameters. */
			sConfigOC3.OCMode = TIM_OCMODE_PWM1;
			sConfigOC3.Pulse = 0;
			sConfigOC3.OCPolarity = TIM_OCPOLARITY_HIGH;
			sConfigOC3.OCFastMode = TIM_OCFAST_ENABLE;
			/* If initialization is inadequate, call for the error callback. */
			if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC3, TIM_CHANNEL_1) != HAL_OK)
			{
			  HAL_TIM_ErrorCallback(&htim3);
			}
			else
			{
			  /* do nothing */
			}
			/* If initialization is inadequate, call for the error callback. */
			if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC3, TIM_CHANNEL_2) != HAL_OK)
			{
			  HAL_TIM_ErrorCallback(&htim3);
			}
			else
			{
			  /* do nothing */
			}
			/* If initialization is inadequate, call for the error callback. */
			if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC3, TIM_CHANNEL_3) != HAL_OK)
			{
			  HAL_TIM_ErrorCallback(&htim3);
			}
			else
			{
			  /* do nothing */
			}
			/* If initialization is inadequate, call for the error callback. */
			if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC3, TIM_CHANNEL_4) != HAL_OK)
			{
			  HAL_TIM_ErrorCallback(&htim3);
			}
			else
			{
			  /* do nothing */
			}
			/* Configure timer GPIO. */
			HAL_TIM_MspPostInit(&htim3);
			break;
		/* Initialize the timer four. */
		case TIMER_FOUR:
			/* Configure timer parameters. */
			htim4.Instance = TIM4;
			htim4.Init.Prescaler = 10000-1;
			htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
			htim4.Init.Period = 1000-1;
			htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
			htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
			/* If initialization is inadequate, call for the error callback. */
			if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
			{
				HAL_TIM_ErrorCallback(&htim4);
			}
			else
			{
				/* do nothing */
			}
			/* Configure the clock source. */
			sClockSourceConfig4.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
			/* If initialization is inadequate, call for the error callback. */
			if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig4) != HAL_OK)
			{
				HAL_TIM_ErrorCallback(&htim4);
			}
			else
			{
				/* do nothing */
			}
			/* If initialization is inadequate, call for the error callback. */
			if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
			{
				HAL_TIM_ErrorCallback(&htim4);
			}
			else
			{
				/* do nothing */
			}
			/* Configure the master slave mode. */
			sMasterConfig4.MasterOutputTrigger = TIM_TRGO_RESET;
			sMasterConfig4.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
			/* If initialization is inadequate, call for the error callback. */
			if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig4) != HAL_OK)
			{
				HAL_TIM_ErrorCallback(&htim4);
			}
			else
			{
				/* do nothing */
			}
			/* Configure the PWM parameters. */
			sConfigOC4.OCMode = TIM_OCMODE_PWM1;
			sConfigOC4.Pulse = 999;
			sConfigOC4.OCPolarity = TIM_OCPOLARITY_HIGH;
			sConfigOC4.OCFastMode = TIM_OCFAST_DISABLE;
			/* If initialization is inadequate, call for the error callback. */
			if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC4, TIM_CHANNEL_3) != HAL_OK)
			{
				HAL_TIM_ErrorCallback(&htim4);
			}
			else
			{
				/* do nothing */
			}
			/* If initialization is inadequate, call for the error callback. */
			if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC4, TIM_CHANNEL_4) != HAL_OK)
			{
				HAL_TIM_ErrorCallback(&htim4);
			}
			else
			{
				/* do nothing */
			}
			/* Configure the timer GPIO. */
			HAL_TIM_MspPostInit(&htim4);
			break;
		/* Initialize the timer five. */
		case TIMER_FIVE:
			htim5.Instance = TIM5;
			htim5.Init.Prescaler = 100-1;
			htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
			htim5.Init.Period = 0xffffffff;
			htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
			htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
			if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
			{
				HAL_TIM_ErrorCallback(&htim5);
			}
			else
			{
				/* do nothing */
			}
			sClockSourceConfig5.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
			if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig5) != HAL_OK)
			{
				HAL_TIM_ErrorCallback(&htim5);
			}
			else
			{
				/* do nothing */
			}

			if (HAL_TIM_IC_Init(&htim5) != HAL_OK)
			{
				HAL_TIM_ErrorCallback(&htim5);
			}
			else
			{
				/* do nothing */
			}
			sMasterConfig5.MasterOutputTrigger = TIM_TRGO_RESET;
			sMasterConfig5.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
			if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig5) != HAL_OK)
			{
				HAL_TIM_ErrorCallback(&htim5);
			}
			else
			{
				/* do nothing */
			}
			sConfigIC5.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
			sConfigIC5.ICSelection = TIM_ICSELECTION_DIRECTTI;
			sConfigIC5.ICPrescaler = TIM_ICPSC_DIV1;
			sConfigIC5.ICFilter = 0;
			if (HAL_TIM_IC_ConfigChannel(&htim5, &sConfigIC5, TIM_CHANNEL_1) != HAL_OK)
			{
				HAL_TIM_ErrorCallback(&htim5);
			}
			else
			{
				/* do nothing */
			}

			if (HAL_TIM_IC_ConfigChannel(&htim5, &sConfigIC5, TIM_CHANNEL_2) != HAL_OK)
			{
				HAL_TIM_ErrorCallback(&htim5);
			}
			else
			{
				/* do nothing */
			}

			if(TimH_MainFunctionFirstCall == STD_LOW)
			{
				HAL_TIM_IC_Start_IT(&htim5, TIM_CHANNEL_1);
				HAL_TIM_IC_Start_IT(&htim5, TIM_CHANNEL_2);
				TimH_MainFunctionFirstCall = STD_HIGH;
			}
			else
			{
				/* do nothing */
			}

			break;
		default:
			break;
	}
	return E_OK;
}
/***********************************************************************************
* END OF Tim_Init										           	               *
************************************************************************************/
/***********************************************************************************
* Function: Tim_DeInit										   			           *
* Description: Peripheral de-initialization.									   *
************************************************************************************/
StdReturnType Tim_DeInit(uint8 TimerChannel)
{
	/* Store the timer channel value into a local variable. */
	uint8 localVariable = TimerChannel;
	/* Process the request for timer de-init. */
	switch(localVariable)
	{
		case TIMER_TWO:
			HAL_TIM_Base_MspDeInit(&htim2);
			break;
		case TIMER_THREE:
			HAL_TIM_Base_MspDeInit(&htim3);
			break;
		case TIMER_FOUR:
			HAL_TIM_Base_MspDeInit(&htim4);
			break;
		case TIMER_FIVE:
			HAL_TIM_Base_MspDeInit(&htim5);
			break;
		default:
			break;
	}
	return E_OK;
}
/***********************************************************************************
* END OF Tim_DeInit										           	   			   *
************************************************************************************/
/***********************************************************************************
* Function: HAL_TIM_ErrorCallback										   		   *
* Description: Error callback called when timer fault occurs.					   *
************************************************************************************/
VOID HAL_TIM_ErrorCallback(TIM_HandleTypeDef *htim)
{
	/* Perform re-initialization of the peripheral in case of error. */
	if(htim->Instance == TIM2)
	{
		SystemManager_Fault[TIMER2_ERROR]++;
		Tim_DeInit(TIMER_TWO);
		Tim_Init(TIMER_TWO);
	}
	else if(htim->Instance == TIM3)
	{
		SystemManager_Fault[TIMER3_ERROR]++;
		Tim_DeInit(TIMER_THREE);
		Tim_Init(TIMER_THREE);
	}
	else if(htim->Instance == TIM4)
	{
		SystemManager_Fault[TIMER4_ERROR]++;
		Tim_DeInit(TIMER_FOUR);
		Tim_Init(TIMER_FOUR);
	}
	else if(htim->Instance == TIM5)
	{
		SystemManager_Fault[TIMER5_ERROR]++;
		Tim_DeInit(TIMER_FIVE);
		Tim_Init(TIMER_FIVE);
	}
	else
	{
		/* do nothing */
	}
}
/***********************************************************************************
* END OF HAL_TIM_ErrorCallback										           	   *
************************************************************************************/
/***********************************************************************************
* Function: Tim_MainFunction										   			   *
* Description: Peripheral main function.									       *
************************************************************************************/
VOID Tim_MainFunction()
{
	/* Check each timer for error status into local variables. */
	uint32 localStateTimerTwo = HAL_TIM_Base_GetState(&htim2);
	uint32 localStateTimerThree = HAL_TIM_Base_GetState(&htim3);
	uint32 localStateTimerFour = HAL_TIM_Base_GetState(&htim4);
	uint32 localStateTimerFive = HAL_TIM_Base_GetState(&htim5);
	/* Process timers state. */
	switch(localStateTimerFive)
	{
		case HAL_TIM_STATE_RESET:
			TimH_BswState_TimerFive = localStateTimerFive;
			Tim_Init(TIMER_FIVE);
			break;
		case HAL_TIM_STATE_READY:
			HAL_GPIO_WritePin(PDCR_TRIG_PORT, PDCR_TRIG_PIN, GPIO_PIN_SET);
			HAL_GPIO_WritePin(PDCR_TRIG_PORT, PDCR_TRIG_PIN, GPIO_PIN_RESET);
			__HAL_TIM_ENABLE_IT(&htim5, TIM_IT_CC2);
			HAL_GPIO_WritePin(PDCF_TRIG_PORT, PDCF_TRIG_PIN, GPIO_PIN_SET);
			HAL_GPIO_WritePin(PDCF_TRIG_PORT, PDCF_TRIG_PIN, GPIO_PIN_RESET);
			__HAL_TIM_ENABLE_IT(&htim5, TIM_IT_CC1);
			TimH_BswState_TimerFive = localStateTimerFive;
			break;
		case HAL_TIM_STATE_BUSY:
			TimH_BswState_TimerFive = localStateTimerFive;
			break;
		case HAL_TIM_STATE_TIMEOUT:
			TimH_BswState_TimerFive = localStateTimerFive;
			HAL_TIM_ErrorCallback(&htim5);
			break;
		case HAL_TIM_STATE_ERROR:
			TimH_BswState_TimerFive = localStateTimerFive;
			HAL_TIM_ErrorCallback(&htim5);
			break;
		default:
			break;
	}
	switch(localStateTimerTwo)
	{
		case HAL_TIM_STATE_RESET:
			TimH_BswState_TimerTwo = localStateTimerTwo;
			Tim_Init(TIMER_TWO);
			break;
		case HAL_TIM_STATE_READY:
			TimH_BswState_TimerTwo = localStateTimerTwo;
			break;
		case HAL_TIM_STATE_BUSY:
			TimH_BswState_TimerTwo = localStateTimerTwo;
			break;
		case HAL_TIM_STATE_TIMEOUT:
			TimH_BswState_TimerTwo = localStateTimerTwo;
			HAL_TIM_ErrorCallback(&htim2);
			break;
		case HAL_TIM_STATE_ERROR:
			TimH_BswState_TimerTwo = localStateTimerTwo;
			HAL_TIM_ErrorCallback(&htim2);
			break;
		default:
			break;
	}
	switch(localStateTimerThree)
	{
		case HAL_TIM_STATE_RESET:
			TimH_BswState_TimerThree = localStateTimerThree;
			Tim_Init(TIMER_THREE);
			break;
		case HAL_TIM_STATE_READY:
			TimH_BswState_TimerThree = localStateTimerThree;
			break;
		case HAL_TIM_STATE_BUSY:
			TimH_BswState_TimerThree = localStateTimerThree;
			break;
		case HAL_TIM_STATE_TIMEOUT:
			TimH_BswState_TimerThree = localStateTimerThree;
			HAL_TIM_ErrorCallback(&htim3);
			break;
		case HAL_TIM_STATE_ERROR:
			TimH_BswState_TimerThree = localStateTimerThree;
			HAL_TIM_ErrorCallback(&htim3);
			break;
		default:
			break;
	}
	switch(localStateTimerFour)
	{
		case HAL_TIM_STATE_RESET:
			TimH_BswState_TimerFour = localStateTimerFour;
			Tim_Init(TIMER_FOUR);
			break;
		case HAL_TIM_STATE_READY:
			TimH_BswState_TimerFour = localStateTimerFour;
			break;
		case HAL_TIM_STATE_BUSY:
			TimH_BswState_TimerFour = localStateTimerFour;
			break;
		case HAL_TIM_STATE_TIMEOUT:
			TimH_BswState_TimerFour = localStateTimerFour;
			HAL_TIM_ErrorCallback(&htim4);
			break;
		case HAL_TIM_STATE_ERROR:
			TimH_BswState_TimerFour = localStateTimerFour;
			HAL_TIM_ErrorCallback(&htim4);
			break;
		default:
			break;
	}
}
/***********************************************************************************
* END OF Tim_MainFunction										           	       *
************************************************************************************/
/***********************************************************************************
* Function: HAL_TIM_PeriodElapsedCallback										   *
* Description: Peripheral period elapsed callback to process system state          *
* 			   when timer expires.									               *
************************************************************************************/
VOID HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	/* Timer 1 is used for the time base source of FreeRTOS. Upon expiration increase the clock counter of the system. */
	if (htim->Instance == TIM1)
	{
		HAL_IncTick();
	}
	else
	{
		/* do nothing */
	}
}
/***********************************************************************************
* END OF HAL_TIM_PeriodElapsedCallback										       *
************************************************************************************/
