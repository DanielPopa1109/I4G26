/*****************************************
* Project: BCM I4						 *
* Developer: Daniel Popa				 *
* Module: Central Locking	     		 *
******************************************/
/*****************************************
*		INCLUDE PATHS					 *
******************************************/
#include "CenLoc.h"
#include "PortH.h"
#include "App_Dependencies.h"
/*****************************************
*		END OF INCLUDE PATHS		     *
******************************************/
/*****************************************
*		VARIABLES					 	 *
******************************************/
/* Central lock current state variable. */
uint8 CenLoc_CurrentState = STD_LOW;
/* Central lock previous current state variable. */
uint8 CenLoc_PreviousState = STD_LOW;
/* Follow me home feature current state variable. */
uint8 CenLoc_FollowMeHomeState = STD_LOW;
/* Central lock lock unlock sequence blink state for turn signals variable. */
uint8 CenLoc_BlinkState = STD_LOW;
/* Timer cycle counter. */
uint8 CenLoc_BlinkCounter = STD_LOW;
/* Timer cycle counter. */
uint8 CenLoc_CyclicAlarmCounter = STD_LOW;
/* Timer cycle counter. */
uint8 CenLoc_FollowMeHomeCounter = STD_LOW;
/* Timer cycle counter. */
uint8 CenLoc_TurnOnLedCounter = STD_LOW;
/* Central lock previous state flag variable. */
uint8 CenLoc_PreviousStateFlag = STD_LOW;
/* Local previous state variable used locking sequence. */
STATIC uint8 localPreviousState = STD_LOW;
/*****************************************
*		END OF VARIABLES				 *
******************************************/
/*****************************************
*		FUNCTIONS				 		 *
******************************************/
/* Central lock main function declaration. */
VOID CenLoc_MainFunction();
/* Buzzer lock unlock state control function declaration. */
VOID CenLoc_ToggleBuzzer(uint8 PinState);
/* Follow me home state processing function declaration. */
VOID CenLoc_FollowMeHome();
/* Follow me home state processing function declaration. */
VOID CenLoc_BlinkSignals();
/* Security alarm turn off function declaration. */
VOID CenLoc_SecAlmStateToOff();
/* Unlock sequence function declaration. */
VOID CenLoc_UnlockSequence();
/* Lock sequence function declaration. */
VOID CenLoc_LockSequence();
/* Security alarm LED control function declaration. */
VOID CenLoc_ControlAlarmLed();
/* Current and previous state update function declaration. */
VOID CenLoc_State();
/* Trigger for lock / unlock sequence function declaration. */
VOID CenLoc_LockUnlockStates();
/*****************************************
*		END OF FUNCTIONS				 *
******************************************/
/***********************************************************************************
* Function: CenLoc_State													   	   *
* Description: Process the current and previous state of the central lock.		   *
************************************************************************************/
VOID CenLoc_State()
{
	/* Every time the state of the central lock changes
	 * set the alarm to off. */
	CenLoc_SecAlmStateToOff();
	/* If the current state is different than the previous state
	 * update the variables respectively.
	 * Initialize the security alarm application.
	 * Calls for start of OS timer for lock unlock sequence.*/
	if(CenLoc_PreviousState != CenLoc_CurrentState)
	{
		CenLoc_PreviousStateFlag 	= CenLoc_PreviousState;
		CenLoc_PreviousState 		= CenLoc_CurrentState;
		CenLoc_BlinkCounter 		= STD_LOW;
		CenLoc_FollowMeHomeCounter 	= STD_LOW;
		CenLoc_CyclicAlarmCounter	= STD_LOW;
		CenLoc_TurnOnLedCounter		= STD_LOW;
		SecAlm_Init();
		osTimerStart(Os_CenLoc_LockUnlockSequence_TimerHandle, 250);
		Os_LockUnlockSequence_Counter = CenLoc_BlinkCounter;
		Os_TurnOnCyclic_Counter = CenLoc_CyclicAlarmCounter;
		Os_TurnOnLed_Counter = CenLoc_TurnOnLedCounter;
		Os_FollowMeHome_Counter = CenLoc_FollowMeHomeCounter;
	}
	else
	{
		/* do nothing */
	}
}
/***********************************************************************************
* END OF CenLoc_SecAlmStateToOff											  	   *
************************************************************************************/
/***********************************************************************************
* Function: CenLoc_SecAlmStateToOff												   *
* Description: Turn off the security alarm.										   *
************************************************************************************/
VOID CenLoc_SecAlmStateToOff()
{
	/* The central lock is on, the security alarm goes off. */
	if(CenLoc_CurrentState == STD_HIGH)
	{
		SecAlm_Init();
	}
	else
	{
		/* do nothing */
	}
}
/***********************************************************************************
* END OF CenLoc_SecAlmStateToOff											  	   * 		   																	       																	   *
************************************************************************************/
/***********************************************************************************
* Function: CenLoc_FollowMeHome												       *
* Description: Process the state of the follow me home.					 		   *
************************************************************************************/
VOID CenLoc_FollowMeHome()
{
	/* Follow me home is on as long as the timer is on. */
	if(CenLoc_FollowMeHomeCounter == 1)
	{
		CenLoc_FollowMeHomeState = STD_HIGH;
	}
	else if(CenLoc_FollowMeHomeCounter == 2)
	{
		osTimerStop(Os_FollowMeHome_TimerHandle);
		CenLoc_FollowMeHomeState = STD_LOW;
		CenLoc_FollowMeHomeCounter = 2;
		Os_FollowMeHome_Counter = CenLoc_FollowMeHomeCounter;
	}
	else
	{
		/* do nothing */
	}
}
/***********************************************************************************
* END OF CenLoc_FollowMeHome											  		   * 		   																	       																	   *
************************************************************************************/
/***********************************************************************************
* Function: CenLoc_UnlockSequence												   *
* Description: Process the unlock sequence. 		   							   *
************************************************************************************/
VOID CenLoc_UnlockSequence()
{
	/* Stop the timer used for the security alarm LED. */
	osTimerStop(Os_SecAlmLed_TurnOnCyclic_TimerHandle);
	/* Process follow me home state. */
	CenLoc_FollowMeHome();
	/* Sets previous state to high so that on locking it is taken into consideration. */
	localPreviousState = STD_HIGH;
	/* Unlocking sequence consists of triggering the hazard lights twice on and twice off.*/
	if(CenLoc_BlinkCounter <= 3)
	{
		/* On and off period for the hazard lights and buzzer is defined in the OS timer start call. */
		osTimerStart(Os_TurnSignals_TimerHandle, 500);
		/* Start the OS timer for the follow me home. */
		if(osTimerIsRunning(Os_FollowMeHome_TimerHandle) == 0)
		{
			osTimerStart(Os_FollowMeHome_TimerHandle, 20000);
		}
		else
		{
			/* do nothing */
		}
		/* Set the follow me home timer state variable to one to prevent activation of the follow me home
		 * when it is not requested. Upon expiration, the timer callback increments this variable. */
		CenLoc_FollowMeHomeCounter = 1;
		Os_FollowMeHome_Counter = CenLoc_FollowMeHomeCounter;
		/* Process the on off states of the hazard lights and the buzzer. */
		switch(CenLoc_BlinkCounter)
		{
			case 0:
				CenLoc_BlinkState = STD_HIGH;
				HAL_TIM_PWM_Start_IT(&htim4, TIM_CHANNEL_3);
				HAL_TIM_PWM_Start_IT(&htim4, TIM_CHANNEL_4);
				CenLoc_ToggleBuzzer(CenLoc_BlinkState);
				break;
			case 1:
				CenLoc_BlinkState = STD_LOW;
				HAL_TIM_PWM_Start_IT(&htim4, TIM_CHANNEL_3);
				HAL_TIM_PWM_Start_IT(&htim4, TIM_CHANNEL_4);
				CenLoc_ToggleBuzzer(CenLoc_BlinkState);
				break;
			case 2:
				CenLoc_BlinkState = STD_HIGH;
				HAL_TIM_PWM_Start_IT(&htim4, TIM_CHANNEL_3);
				HAL_TIM_PWM_Start_IT(&htim4, TIM_CHANNEL_4);
				CenLoc_ToggleBuzzer(CenLoc_BlinkState);
				break;
			case 3:
				CenLoc_BlinkState = STD_LOW;
				HAL_TIM_PWM_Start_IT(&htim4, TIM_CHANNEL_3);
				HAL_TIM_PWM_Start_IT(&htim4, TIM_CHANNEL_4);
				CenLoc_ToggleBuzzer(CenLoc_BlinkState);
				break;
			default:
				break;
		}
	}/* After the sequence is completed, stop the timers and set the variables related to this sequence to out of bounds values to prevent triggering when not requested. */
	else if(CenLoc_BlinkCounter >= 3)
	{
		CenLoc_BlinkState = 2;
		CenLoc_BlinkCounter = 6;
		osTimerStop(Os_CenLoc_LockUnlockSequence_TimerHandle);
	}
	else
	{
		/* do nothing */
	}
}
/***********************************************************************************
* END OF CenLoc_UnlockSequence											  		   * 		   																	       																	   *
************************************************************************************/
/***********************************************************************************
* Function: CenLoc_LockSequence													   *
* Description: Process the lock sequence.								 		   *
************************************************************************************/
VOID CenLoc_LockSequence()
{
	/* Process follow me home state.*/
	CenLoc_FollowMeHome();
	/* If the central lock has been on previously
	 * and the variable related to the state of the timer is less than the value of
	 * how many times the hazard lights should turn on on locking, proceed with the processing. */
	if(CenLoc_BlinkCounter <= 1 && localPreviousState == STD_HIGH)
	{
		/* On and off period for the hazard lights and buzzer is defined in the OS timer start call. */
		osTimerStart(Os_TurnSignals_TimerHandle, 500);
		/* Start the OS timer for the follow me home. */
		if(osTimerIsRunning(Os_FollowMeHome_TimerHandle) == 0)
		{
			osTimerStart(Os_FollowMeHome_TimerHandle, 20000);
		}
		else
		{
			/* do nothing */
		}
		/* Set the follow me home timer state variable to one to prevent activation of the follow me home
		 * when it is not requested. Upon expiration, the timer callback increments this variable. */
		CenLoc_FollowMeHomeCounter = 1;
		Os_FollowMeHome_Counter = CenLoc_FollowMeHomeCounter;
		/* Process the on off states of the hazard lights and buzzer. */
		switch(CenLoc_BlinkCounter)
		{
			case 0:
				CenLoc_BlinkState = STD_HIGH;
				HAL_TIM_PWM_Start_IT(&htim4, TIM_CHANNEL_3);
				HAL_TIM_PWM_Start_IT(&htim4, TIM_CHANNEL_4);
				CenLoc_ToggleBuzzer(CenLoc_BlinkState);
				break;
			case 1:
				CenLoc_BlinkState = STD_LOW;
				HAL_TIM_PWM_Start_IT(&htim4, TIM_CHANNEL_3);
				HAL_TIM_PWM_Start_IT(&htim4, TIM_CHANNEL_4);
				CenLoc_ToggleBuzzer(CenLoc_BlinkState);
				localPreviousState = STD_LOW;
				break;
			default:
				break;
		}
	}/* After the sequence is completed, stop the timers and set the variables related to this sequence to out of bounds values to prevent triggering when not requested. */
	else if(CenLoc_BlinkCounter == 2)
	{
		CenLoc_BlinkState = 2;
		CenLoc_BlinkCounter = 4;
		Os_LockUnlockSequence_Counter = CenLoc_BlinkCounter;
		osTimerStop(Os_CenLoc_LockUnlockSequence_TimerHandle);
	}
	else
	{
		/* do nothing */
	}
}
/***********************************************************************************
* END OF CenLoc_LockSequence											  		   * 		   																	       																	   *
************************************************************************************/
/***********************************************************************************
* Function: CenLoc_ControlAlarmLed												   *
* Description: Controls the security alarm LED 		   							   *
************************************************************************************/
VOID CenLoc_ControlAlarmLed()
{
	/* Check if the central lock state is set to high. */
	if(CenLoc_CurrentState == STD_HIGH)
	{
		/* Set the security alarm LED to off. */
		SecAlm_ToggleAlarmLed(STD_LOW);
		/* Set the timer flag to 0. */
		CenLoc_TurnOnLedCounter = STD_LOW;
		/* Stop the timer related to the LED cyclic trigger. */
		osTimerStop(Os_SecAlmLedTurnOn_TimerHandle);
	}/* Check if the central lock state is set to low. */
	else if(CenLoc_CurrentState == STD_LOW)
	{
		/* Check if the security alarm LED cyclic trigger OS timer is running and the OS timer on period are off. */
		if(osTimerIsRunning(Os_SecAlmLed_TurnOnCyclic_TimerHandle) == 0 && osTimerIsRunning(Os_SecAlmLedTurnOn_TimerHandle) == 0)
		{
			/* Start the security alarm LED cyclic trigger OS timer. */
			osTimerStart(Os_SecAlmLed_TurnOnCyclic_TimerHandle, 3750);
		}
		else
		{
			/* do nothing */
		}
		/* If the timer for cyclic trigger expired. */
		if(CenLoc_CyclicAlarmCounter == 1)
		{
			/* Check if the OS timer on period is off. */
			if(osTimerIsRunning(Os_SecAlmLedTurnOn_TimerHandle) == 0)
			{
				/* Start the timer with the period set for the LED to be on. */
				osTimerStart(Os_SecAlmLedTurnOn_TimerHandle, 250);
			}
			else
			{
				/* do nothing */
			}
			/* If the timer is not finished turn on the LED.*/
			if(CenLoc_TurnOnLedCounter < 1)
			{
				SecAlm_ToggleAlarmLed(STD_HIGH);
			}/* If the timer expired. */
			else if(CenLoc_TurnOnLedCounter == 1)
			{
				/* Turn off the LED. */
				SecAlm_ToggleAlarmLed(STD_LOW);
				/* Reset the flag. */
				CenLoc_TurnOnLedCounter = 0;
				/* Reset the flag. */
				CenLoc_CyclicAlarmCounter = 0;
				Os_TurnOnCyclic_Counter = CenLoc_CyclicAlarmCounter;
				Os_TurnOnLed_Counter = CenLoc_TurnOnLedCounter;
				/* Stop the OS timer on period. */
				osTimerStop(Os_SecAlmLedTurnOn_TimerHandle);
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
}
/***********************************************************************************
* END OF CenLoc_ControlAlarmLed											  		   * 		   																	       																	   *
************************************************************************************/
/***********************************************************************************
* Function: CenLoc_LockUnlockStates												   *
* Description: Check for the state of the central lock.					 		   *
************************************************************************************/
VOID CenLoc_LockUnlockStates()
{
	/* If the central lock state is set to high and the security alarm is set to off. */
	if(CenLoc_CurrentState == STD_HIGH && SecAlm_Trigger == STD_LOW)
	{
		/* Perform the related activities. */
		CenLoc_UnlockSequence();
		CenLoc_ControlAlarmLed();
	}
	else if(CenLoc_CurrentState == STD_LOW && SecAlm_Trigger == STD_LOW)
	{
		/* Perform the related activities. */
		CenLoc_LockSequence();
		CenLoc_ControlAlarmLed();
	}
	else
	{
		/* do nothing */
	}
}
/***********************************************************************************
* END OF CenLoc_LockUnlockStates											  	   * 		   																	       																	   *
************************************************************************************/
/***********************************************************************************
* Function: CenLoc_ToggleBuzzer													   *
* Description: Trigger central lock buzzer. 		   							   *
************************************************************************************/
VOID CenLoc_ToggleBuzzer(uint8 PinState)
{
	HAL_GPIO_WritePin(CENLOC_BUZZER_PORT, CENLOC_BUZZER_PIN, PinState);
}
/***********************************************************************************
* END OF CenLoc_ToggleBuzzer											  		   * 		   																	       																	   *
************************************************************************************/
/***********************************************************************************
* Function: CenLoc_MainFunction													   *
* Description: Central lock main function. Process the states of the application.  *
************************************************************************************/
VOID CenLoc_MainFunction()
{
	CenLoc_State();
	CenLoc_LockUnlockStates();
}
/***********************************************************************************
* END OF CenLoc_MainFunction											  		   * 		   																	       																	   *
************************************************************************************/
