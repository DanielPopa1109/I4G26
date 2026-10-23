/*****************************************
* Project: BCM I4						 *
* Developer: Daniel Popa				 *
* Module: Interior Lights	     		 *
******************************************/
/*****************************************
*		INCLUDE PATHS					 *
******************************************/
#include "IntLights.h"
#include "App_Dependencies.h"
/*****************************************
*		END OF INCLUDE PATHS		     *
******************************************/
/*****************************************
*		DEFINES					 		 *
******************************************/
/*****************************************
* 		END OF DEFINES					 *
******************************************/
/*****************************************
*		VARIABLES					 	 *
******************************************/
/* Variable for the interior lights current states. */
uint8 IntLights_CurrentState = STD_LOW;
/*****************************************
*		END OF VARIABLES				 *
******************************************/
/*****************************************
*		FUNCTIONS				 		 *
******************************************/
/* Interior lights main function. */
VOID IntLights_MainFunction();
/* Interior lights command processed. */
VOID IntLights_RxBtcState();
/*****************************************
*		END OF FUNCTIONS				 *
******************************************/
/***********************************************************************************
* Function: IntLights_RxBtcState										   		   *
* Description: Process command. 		   		   						           *
************************************************************************************/
VOID IntLights_RxBtcState()
{
	/* Process the state of the interior lights according to system state. */
	if(CenLoc_FollowMeHomeState == STD_HIGH)
	{
		IntLights_CurrentState = STD_HIGH;
	}
	else
	{
		/* do nothing */
	}
	if(CenLoc_FollowMeHomeState == STD_LOW && Btc_IntLights == STD_LOW)
	{
		IntLights_CurrentState = STD_LOW;
	}
	else
	{
		/* do nothing */
	}
	if(CenLoc_FollowMeHomeState == STD_LOW && Btc_IntLights == STD_HIGH)
	{
		IntLights_CurrentState = STD_HIGH;
	}
	else
	{
		/* do nothing */
	}
	HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_3);
}
/***********************************************************************************
* END OF IntLights_RxBtcState										               *
************************************************************************************/
/***********************************************************************************
* Function: IntLights_MainFunction										           *
* Description: Interior lights main function for the application and state process.*
************************************************************************************/
VOID IntLights_MainFunction()
{
	IntLights_RxBtcState();
}
/***********************************************************************************
* END OF IntLights_MainFunction										   			   *
************************************************************************************/
