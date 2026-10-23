/*****************************************
* Project: BCM I4						 *
* Developer: Daniel Popa				 *
* Module: Exterior Lights	     		 *
******************************************/
/*****************************************
*		INCLUDE PATHS					 *
******************************************/
#include "ExtLights.h"
#include "PortH.h"
#include "App_Dependencies.h"
/*****************************************
		END OF INCLUDE PATHS		     *
******************************************/
/*****************************************
*		DEFINES					 		 *
******************************************/
#define EXTLIGHTS_LS_REQUEST 		0x00
/*****************************************
* 		END OF DEFINES					 *
******************************************/
/*****************************************
*		VARIABLES					 	 *
******************************************/
/* Variable to store the application state. */
uint8 ExtLights_ApplState = STD_LOW;
/* Variable for the current state of reverse light. */
uint8 ExtLights_ReverseLight_CurrentState = STD_LOW;
/* Variable for the current state of flashing the high beam. */
uint8 ExtLights_FlashHighBeam_CurrentState = STD_LOW;
/* Variable for the current state of brake light. */
uint8 ExtLights_BrakeLight_CurrentState = STD_LOW;
/* Variable for the current state of light switch. */
uint8 ExtLights_LightsSwitch_CurrentState = STD_LOW;
/* Variable for the current state of high beam. */
uint8 ExtLights_HighBeam_CurrentState = STD_LOW;
/* Variable for the current state of front fog light. */
uint8 ExtLights_FrontFogLight_CurrentState = STD_LOW;
/* Variable for the current state of left turn signals. */
uint8 ExtLights_TurnSignalLeft_CurrentState = STD_LOW;
/* Variable for the current state of right turn signals. */
uint8 ExtLights_TurnSignalRight_CurrentState = STD_LOW;
/* Variable for the current state of the low beam */
uint8 ExtLights_LowBeam_CurrentState = STD_LOW;
/* Variable for the current state of the rear position lights. */
uint8 ExtLights_RearPositionLights_CurrentState = STD_LOW;
/* Variable for the current state of hazard lights. */
uint8 ExtLights_HazardLight_CurrentState = STD_LOW;
/* Variable for the current state of rear fog lights. */
uint8 ExtLights_RearFogLight_CurrentState = STD_LOW;
/* Variable for the right turn signal previous state. */
uint8 ExtLights_RTS_PrevState = STD_LOW;
/* Variable for the left turn signal previous state. */
uint8 ExtLights_LTS_PrevState = STD_LOW;
/* Variable for the hazard light previous state. */
uint8 ExtLights_HL_PrevState = STD_LOW;
/* Variable for the right turn signal counter flag. */
uint32 ExtLights_RTSFlag = STD_LOW;
/* Variable for the left turn signal counter flag. */
uint32 ExtLights_LTSFlag = STD_LOW;
/* Variable for the hazard light counter flag. */
uint32 ExtLights_HLFlag = STD_LOW;
/* Variable for sensor state. */
uint32 ExtLights_LightSensorState = STD_LOW;
/* Static variable for last state. */
STATIC uint8 ExtLights_Previous_RearFogLight_CurrentState = STD_LOW;
/* Static variable for last state. */
STATIC uint8 ExtLights_Previous_FrontFogLight_CurrentState = STD_LOW;
/* Static variable for last state. */
STATIC uint8 ExtLights_Previous_BrakeLight_CurrentState = STD_LOW;
/* Static variable for last state. */
STATIC uint8 ExtLights_Previous_LightSwitchState = STD_LOW;
/*****************************************
*		END OF VARIABLES				 *
******************************************/
/*****************************************
*		FUNCTIONS				 		 *
******************************************/
/* Exterior lights main function declaration. */
VOID ExtLights_MainFunction();
/* High beam trigger function declaration. */
VOID ExtLights_HighBeam(uint8 PinState);
/* Reverse light trigger function declaration. */
VOID ExtLights_ReverseLight(uint8 PinState);
/* Lights states processing function declaration. */
VOID ExtLights_LightState();
/* Turn signals and hazard lights control function declaration. */
VOID ExtLights_TurnSignalHazardLight();
/* Light switch control function declaration. */
VOID ExtLights_LightSwitchMode();
/* Turn signals and hazard lights current and previous state processing function declaration. */
VOID ExtLights_PrevStateTSHL();
/*****************************************
*		END OF FUNCTIONS				 *
******************************************/
/***********************************************************************************
* Function: ExtLights_TurnSignalHazardLight										   *
* Description: Process the turn signals and hazard lights states. 		   		   *
************************************************************************************/
VOID ExtLights_TurnSignalHazardLight()
{
	/* If the turn signal left is set on. */
	if(ExtLights_TurnSignalLeft_CurrentState == STD_HIGH)
	{
		/* Check if the timer related to the turn signals is off. */
		if(osTimerIsRunning(Os_TurnSignals_TimerHandle) == 0)
		{
			/* Start the timer with the designated period counter value. */
			osTimerStart(Os_TurnSignals_TimerHandle, 500);
		}
		else
		{
			/* do nothing */
		}
		/* Turn on and off the turn signal. */
		if(ExtLights_LTSFlag % 2 == 1)
		{
			HAL_TIM_PWM_Start_IT(&htim4, TIM_CHANNEL_3);
		}
		else if(ExtLights_LTSFlag % 2 == 0)
		{
			HAL_TIM_PWM_Stop_IT(&htim4, TIM_CHANNEL_3);
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
	/* If the turn signal right is set on. */
	if(ExtLights_TurnSignalRight_CurrentState == STD_HIGH)
	{
		/* Check if the timer related to the turn signals is off. */
		if(osTimerIsRunning(Os_TurnSignals_TimerHandle) == 0)
		{
			/* Start the timer with the designated period counter value. */
			osTimerStart(Os_TurnSignals_TimerHandle, 500);
		}
		else
		{
			/* do nothing */
		}
		/* Turn on and off the turn signal. */
		if(ExtLights_RTSFlag % 2 == 1)
		{
			HAL_TIM_PWM_Start_IT(&htim4, TIM_CHANNEL_4);
		}
		else if(ExtLights_RTSFlag % 2 == 0)
		{
			HAL_TIM_PWM_Stop_IT(&htim4, TIM_CHANNEL_4);
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
	/* If the hazard light is set on. */
	if(ExtLights_HazardLight_CurrentState == STD_HIGH)
	{
		/* Check if the timer related to the turn signals is off. */
		if(osTimerIsRunning(Os_TurnSignals_TimerHandle) == 0)
		{
			/* Start the timer with the designated period counter value. */
			osTimerStart(Os_TurnSignals_TimerHandle, 500);
		}
		else
		{
			/* do nothing */
		}
		/* Turn on and off the turn signal. */
		if(ExtLights_HLFlag % 2 == 1)
		{
			HAL_TIM_PWM_Start_IT(&htim4, TIM_CHANNEL_3);
			HAL_TIM_PWM_Start_IT(&htim4, TIM_CHANNEL_4);
		}
		else if(ExtLights_HLFlag % 2 == 0)
		{
			HAL_TIM_PWM_Stop_IT(&htim4, TIM_CHANNEL_3);
			HAL_TIM_PWM_Stop_IT(&htim4, TIM_CHANNEL_4);
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
	/* Only one pair of turn signals can be on at a time. Set on one pair and the other off. */
	if(ExtLights_TurnSignalRight_CurrentState == STD_LOW && ExtLights_HazardLight_CurrentState == STD_LOW)
	{
		HAL_TIM_PWM_Stop_IT(&htim4, TIM_CHANNEL_4);
	}
	else
	{
		/* do nothing */
	}
	/* Only one pair of turn signals can be on at a time. Set on one pair and the other off. */
	if(ExtLights_TurnSignalLeft_CurrentState == STD_LOW && ExtLights_HazardLight_CurrentState == STD_LOW)
	{
		HAL_TIM_PWM_Stop_IT(&htim4, TIM_CHANNEL_3);
	}
	else
	{
		/* do nothing */
	}
}
/***********************************************************************************
* END OF ExtLights_TurnSignalHazardLight										   *													       																	   *
************************************************************************************/
/***********************************************************************************
* Function: ExtLights_PrevStateTSHL										           *
* Description: Checks the current state and previous state of turn signals and     *
* 			   hazard lights. 		   											   *
************************************************************************************/
VOID ExtLights_PrevStateTSHL()
{
	/* Change the variables related to states. */
	if(ExtLights_TurnSignalLeft_CurrentState != ExtLights_LTS_PrevState)
	{
		ExtLights_LTS_PrevState = ExtLights_TurnSignalLeft_CurrentState;
		ExtLights_LTSFlag = 0;
		Os_Lts_Counter = ExtLights_LTSFlag;
	}
	else
	{
		/* do nothing */
	}
	/* Change the variables related to states. */
	if(ExtLights_TurnSignalRight_CurrentState != ExtLights_RTS_PrevState)
	{
		ExtLights_RTS_PrevState = ExtLights_TurnSignalRight_CurrentState;
		ExtLights_RTSFlag = 0;
		Os_Rts_Counter = ExtLights_RTSFlag;
	}
	else
	{
		/* do nothing */
	}
	/* Change the variables related to states. */
	if(ExtLights_HazardLight_CurrentState != ExtLights_HL_PrevState)
	{
		ExtLights_HL_PrevState = ExtLights_HazardLight_CurrentState;
		ExtLights_HLFlag = 0;
		Os_HL_Counter = ExtLights_HLFlag;
	}
	else
	{
		/* do nothing */
	}
}
/***********************************************************************************
* END OF ExtLights_PrevStateTSHL										   		   *													       																	   *
************************************************************************************/
/***********************************************************************************
* Function: ExtLights_LightState										   		   *
* Description: Controls the lights states.								 		   *
************************************************************************************/
VOID ExtLights_LightState()
{
	/* Process turn signals state. */
	ExtLights_PrevStateTSHL();
	/* Follow me home is on, then turn on the respective lights. */
	if(CenLoc_FollowMeHomeState == STD_HIGH)
	{
		ExtLights_LightSwitchMode();
		HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_1);
		HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_2);
		ExtLights_LowBeam_CurrentState = STD_HIGH;
		ExtLights_RearPositionLights_CurrentState = STD_HIGH;
	}/* Follow me home is off, turn off the lights.*/
	else if(CenLoc_FollowMeHomeState == STD_LOW)
	{
		ExtLights_LightSwitchMode();
		if(ExtLights_LightsSwitch_CurrentState == STD_LOW)
		{
			HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_1);
			HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_2);
			ExtLights_LowBeam_CurrentState = STD_LOW;
			ExtLights_RearPositionLights_CurrentState = STD_LOW;
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
	/* If any light state is set to on, then stop the follow me home. */
	if(ExtLights_BrakeLight_CurrentState == STD_HIGH ||
		ExtLights_FlashHighBeam_CurrentState == STD_HIGH ||
		ExtLights_LightsSwitch_CurrentState != STD_LOW ||
		ExtLights_HighBeam_CurrentState == STD_HIGH ||
		ExtLights_TurnSignalLeft_CurrentState == STD_HIGH ||
		ExtLights_TurnSignalRight_CurrentState == STD_HIGH ||
		ExtLights_HazardLight_CurrentState == STD_HIGH)
	{
		if(ExtLights_LightsSwitch_CurrentState == STD_LOW)
		{
			CenLoc_FollowMeHomeState = STD_LOW;
			CenLoc_FollowMeHomeCounter = 2;
			Os_FollowMeHome_Counter = 2;
			HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_1);
			HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_2);
			ExtLights_LowBeam_CurrentState = STD_LOW;
			ExtLights_RearPositionLights_CurrentState = STD_LOW;
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
	/* Control the turn signals for the central lock lock unlock sequence when requested. */
	if(CenLoc_BlinkState == STD_HIGH)
	{
		HAL_TIM_PWM_Start_IT(&htim4, TIM_CHANNEL_3);
		HAL_TIM_PWM_Start_IT(&htim4, TIM_CHANNEL_4);
	}
	else if(CenLoc_BlinkState == STD_LOW)
	{
		HAL_TIM_PWM_Stop_IT(&htim4, TIM_CHANNEL_3);
		HAL_TIM_PWM_Stop_IT(&htim4, TIM_CHANNEL_4);
	}/* Allows for turning on the hazard lights or turn signals independently of other states in the system. */
	else if(CenLoc_BlinkState == 2 || CenLoc_BlinkState == 4)
	{
		ExtLights_TurnSignalHazardLight();
	}
	else
	{
		/* do nothing */
	}
	/* Turn on/off high beam. */
	if(ExtLights_FlashHighBeam_CurrentState == STD_HIGH)
	{
		ExtLights_HighBeam(STD_HIGH);
	}
	else if(ExtLights_FlashHighBeam_CurrentState == STD_LOW)
	{
		ExtLights_HighBeam(STD_LOW);
	}
	else
	{
		/* do nothing */
	}
	/* Turn on/off high beam. */
	if(ExtLights_HighBeam_CurrentState == STD_HIGH)
	{
		ExtLights_HighBeam(STD_HIGH);
	}
	else if(ExtLights_HighBeam_CurrentState == STD_LOW)
	{
		ExtLights_HighBeam(STD_LOW);
	}
	else
	{
		/* do nothing */
	}
	/* Turn on or off the brake lights. */
	if(ExtLights_BrakeLight_CurrentState == STD_HIGH)
	{
		if(ExtLights_BrakeLight_CurrentState != ExtLights_Previous_BrakeLight_CurrentState)
		{
			ExtLights_Previous_BrakeLight_CurrentState = ExtLights_BrakeLight_CurrentState;
			TIM3->CCR2 = (0);
		}
		else
		{
			/* do nothing */
		}
		HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_2);
	}
	else if(ExtLights_BrakeLight_CurrentState == STD_LOW)
	{
		if(ExtLights_BrakeLight_CurrentState != ExtLights_Previous_BrakeLight_CurrentState)
		{
			ExtLights_Previous_BrakeLight_CurrentState = ExtLights_BrakeLight_CurrentState;
			TIM3->CCR2 = (0);
		}
		else
		{
			/* do nothing */
		}
		HAL_TIM_PWM_Stop_IT(&htim3, TIM_CHANNEL_2);
	}
	else
	{
		/* do nothing */
	}
	/* Turn on or off the rear fog lights. */
	if(ExtLights_RearFogLight_CurrentState == STD_HIGH)
	{
		if(ExtLights_RearFogLight_CurrentState != ExtLights_Previous_RearFogLight_CurrentState)
		{
			ExtLights_Previous_RearFogLight_CurrentState = ExtLights_RearFogLight_CurrentState;
			TIM3->CCR4 = (0);
		}
		else
		{
			/* do nothing */
		}
		HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_4);
	}
	else if(ExtLights_RearFogLight_CurrentState == STD_LOW)
	{
		if(ExtLights_RearFogLight_CurrentState != ExtLights_Previous_RearFogLight_CurrentState)
		{
			ExtLights_Previous_RearFogLight_CurrentState = ExtLights_RearFogLight_CurrentState;
			TIM3->CCR4 = (0);
		}
		else
		{
			/* do nothing */
		}
		HAL_TIM_PWM_Stop_IT(&htim3, TIM_CHANNEL_4);
	}
	else
	{
		/* do nothing */
	}
	/* Turn on or off the front fog lights. */
	if(ExtLights_FrontFogLight_CurrentState == STD_HIGH)
	{
		if(ExtLights_FrontFogLight_CurrentState != ExtLights_Previous_FrontFogLight_CurrentState)
		{
			ExtLights_Previous_FrontFogLight_CurrentState = ExtLights_FrontFogLight_CurrentState;
			TIM3->CCR3 = (0);
		}
		else
		{
			/* do nothing */
		}
		HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_3);
	}
	else if(ExtLights_FrontFogLight_CurrentState == STD_LOW)
	{
		if(ExtLights_FrontFogLight_CurrentState != ExtLights_Previous_FrontFogLight_CurrentState)
		{
			ExtLights_Previous_FrontFogLight_CurrentState = ExtLights_FrontFogLight_CurrentState;
			TIM3->CCR3 = (0);
		}
		else
		{
			/* do nothing */
		}
		HAL_TIM_PWM_Stop_IT(&htim3, TIM_CHANNEL_3);
	}
	else
	{
		/* do nothing */
	}
	/* Turn on the reverse light. */
	ExtLights_ReverseLight(ExtLights_ReverseLight_CurrentState);

	if(CenLoc_CurrentState == STD_LOW)
	{
		ExtLights_ReverseLight_CurrentState = STD_LOW;
		ExtLights_RearFogLight_CurrentState = STD_LOW;
		ExtLights_FrontFogLight_CurrentState = STD_LOW;
		ExtLights_HighBeam_CurrentState = STD_LOW;
		ExtLights_BrakeLight_CurrentState = STD_LOW;
	}
	else
	{
		/* do nothing */
	}
}
/***********************************************************************************
* END OF ExtLights_LightState										   			   *													       																	   *
************************************************************************************/
/***********************************************************************************
* Function: ExtLights_MainFunction										   		   *
* Description: Exterior lights application main function. Controls application	   *
* 			   states. 		   													   *
************************************************************************************/
VOID ExtLights_MainFunction()
{
	ExtLights_LightState();
}
/***********************************************************************************
* END OF ExtLights_MainFunction										   			   *													       																	   *
************************************************************************************/
/***********************************************************************************
* Function: ExtLights_LightSwitchMode										   	   *
* Description: Control the light switch.								 		   *
************************************************************************************/
VOID ExtLights_LightSwitchMode()
{
	/* Switch through the light switch states. */
	switch(ExtLights_LightsSwitch_CurrentState)
	{
		/* Switch is on position zero, turn off the lights. */
		case EXTLIGHTS_LIGHTSWITCH_STATEZERO:
			if(ExtLights_Previous_LightSwitchState != ExtLights_LightsSwitch_CurrentState)
			{
				ExtLights_Previous_LightSwitchState = ExtLights_LightsSwitch_CurrentState;
				TIM2->CCR1 = (0);
				TIM2->CCR2 = (0);
				ExtLights_LowBeam_CurrentState = STD_LOW;
				ExtLights_RearPositionLights_CurrentState = STD_LOW;
			}
			else
			{
				/* do nothing */
			}
			HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_1);
			HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_2);
			ExtLights_LowBeam_CurrentState = STD_LOW;
			ExtLights_RearPositionLights_CurrentState = STD_LOW;
			break;
		/* Switch is on position one, turn automatic lights controlled by sensor's input. */
		case EXTLIGHTS_LIGHTSWITCH_STATEONE:
			/* Read the sensor status. */
			if(Os_Counter % 39 == 0)
			{
				SenCtrl_ProcessSensorValue(EXTLIGHTS_LS_REQUEST);
			}
			else
			{
				/* do nothing */
			}
			if(ExtLights_Previous_LightSwitchState != ExtLights_LightsSwitch_CurrentState)
			{
				ExtLights_Previous_LightSwitchState = ExtLights_LightsSwitch_CurrentState;
				TIM2->CCR1 = (0);
				TIM2->CCR2 = (0);
				ExtLights_LowBeam_CurrentState = STD_LOW;
				ExtLights_RearPositionLights_CurrentState = STD_LOW;
			}
			else
			{
				/* do nothing */
			}
			/* If the sensor status is on, turn on the lights to on. */
			if(ExtLights_LightSensorState == 1)
			{
				ExtLights_LowBeam_CurrentState = STD_HIGH;
				ExtLights_RearPositionLights_CurrentState = STD_HIGH;
				HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_1);
				HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_2);
			}/* Else turn them off. */
			else if(ExtLights_LightSensorState != 1)
			{
				ExtLights_LowBeam_CurrentState = STD_LOW;
				ExtLights_RearPositionLights_CurrentState = STD_LOW;
				HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_1);
				HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_2);
			}
			break;
		/* Switch is on position three, turn on position lights. */
		case EXTLIGHTS_LIGHTSWITCH_STATETWO:
			if(ExtLights_Previous_LightSwitchState != ExtLights_LightsSwitch_CurrentState)
			{
				ExtLights_Previous_LightSwitchState = ExtLights_LightsSwitch_CurrentState;
				TIM2->CCR1 = (0);
				TIM2->CCR2 = (0);
				ExtLights_LowBeam_CurrentState = STD_LOW;
				ExtLights_RearPositionLights_CurrentState = STD_LOW;
			}
			else
			{
				/* do nothing */
			}
			HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_1);
			HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_2);
			TIM2->CCR1 = (750);
			TIM2->CCR2 = (1500);
			ExtLights_LowBeam_CurrentState = STD_LOW;
			ExtLights_RearPositionLights_CurrentState = STD_LOW;
			break;
		/* Switch is on position four, turn on night time lights. */
		case EXTLIGHTS_LIGHTSWITCH_STATETHREE:
			if(ExtLights_Previous_LightSwitchState != ExtLights_LightsSwitch_CurrentState)
			{
				ExtLights_Previous_LightSwitchState = ExtLights_LightsSwitch_CurrentState;
				TIM2->CCR1 = (0);
				TIM2->CCR2 = (0);
				ExtLights_LowBeam_CurrentState = STD_LOW;
				ExtLights_RearPositionLights_CurrentState = STD_LOW;
			}
			else
			{
				/* do nothing */
			}
			HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_1);
			HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_2);
			ExtLights_LowBeam_CurrentState = STD_HIGH;
			ExtLights_RearPositionLights_CurrentState = STD_HIGH;
			break;
		default:
			break;
	}
}
/***********************************************************************************
* END OF ExtLights_LightSwitchMode										   		   *													       																	   *
************************************************************************************/
/***********************************************************************************
* Function: ExtLights_HighBeam										   			   *
* Description: High beam trigger. 		  										   *
************************************************************************************/
VOID ExtLights_HighBeam(uint8 PinState)
{
	HAL_GPIO_WritePin(EXTLIGHTS_HIGHBEAM_PORT, EXTLIGHTS_HIGHBEAM_PIN, PinState);
}
/***********************************************************************************
* END OF ExtLights_HighBeam										   				   *													       																	   *
************************************************************************************/
/***********************************************************************************
* Function: ExtLights_ReverseLight										   		   *
* Description: Reverse light trigger. 		   									   *
************************************************************************************/
VOID ExtLights_ReverseLight(uint8 PinState)
{
	HAL_GPIO_WritePin(EXTLIGHTS_REVERSELIGHT_PORT, EXTLIGHTS_REVERSELIGHT_PIN, PinState);
}
/***********************************************************************************
* END OF ExtLights_ReverseLight										   			   *													       																	   *
************************************************************************************/
