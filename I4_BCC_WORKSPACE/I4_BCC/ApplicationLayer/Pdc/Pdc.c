/*****************************************
* Project: BCM I4						 *
* Developer: Daniel Popa				 *
* Module: Parking Distance Control 	     *
******************************************/
/*****************************************
*		INCLUDE PATHS					 *
******************************************/
#include "Pdc.h"
#include "PortH.h"
#include "SystemManager.h"
#include "TimH.h"
#include "App_Dependencies.h"
/*****************************************
*		END OF INCLUDE PATHS		     *
******************************************/
/*****************************************
*		DEFINES					 		 *
******************************************/
/* Safety distance define */
#define PDC_MAX_DISTANCE			20
/* Safety distance define */
#define PDC_FIRST_SAFE_DISTANCE		17
/* Safety distance define */
#define PDC_SECOND_SAFE_DISTANCE	14
/* Safety distance define */
#define PDC_THIRD_SAFE_DISTANCE		11
/* Timer period. */
#define PDC_BUZZER_PERIOD_ONE		500
/* Timer period. */
#define PDC_BUZZER_PERIOD_TWO		250
/* Timer period. */
#define PDC_BUZZER_PERIOD_THREE		125
/* Timer period. */
#define PDC_BUZZER_PERIOD_FOUR		75
/*****************************************
* 		END OF DEFINES					 *
******************************************/
/*****************************************
*		VARIABLES					 	 *
******************************************/
/* Variable to store rear distance. */
uint8 Pdc_Rear_Distance = STD_LOW;
/* Variable to store front distance. */
uint8 Pdc_Front_Distance = STD_LOW;
/* Variable to store the rear buzzer counter. */
uint32 Pdc_Rear_BuzzerOsCounter = STD_LOW;
/* Variable to store the front buzzer counter. */
uint32 Pdc_Front_BuzzerOsCounter = STD_LOW;
/* Static variable for counting. */
STATIC uint32 PdcR_DistanceRange = STD_LOW;
/* Static variable for counting. */
STATIC uint32 PdcF_DistanceRange = STD_LOW;
/*****************************************
*		END OF VARIABLES				 *
******************************************/
/*****************************************
*		FUNCTIONS				 		 *
******************************************/
/* Main function declaration. */
VOID Pdc_MainFunction();
/* Distance process data function declaration. */
VOID Pdc_Rear_ProcessData();
/* Distance process data function declaration. */
VOID Pdc_Front_ProcessData();
/* Buzzer control function declaration. */
VOID Pdc_Rear_TriggerBuzzer();
/* Buzzer control function declaration. */
VOID Pdc_Front_TriggerBuzzer();
/*****************************************
*		END OF FUNCTIONS				 *
******************************************/
/***********************************************************************************
* Function: Pdc_MainFunction													   *
* Description: Main function application. 		   			                       *
************************************************************************************/
VOID Pdc_MainFunction()
{
	if(Btc_ReverseLight == STD_HIGH)
	{
		Pdc_Front_ProcessData();
		Pdc_Rear_ProcessData();
	}
	else
	{
		/* do nothing */
	}
}
/***********************************************************************************
* END OF Pdc_MainFunction											  			   * 		   																	       																	   *
************************************************************************************/
/***********************************************************************************
* Function: Pdc_Rear_ProcessData												   *
* Description: Process distance data.		   			                           *
************************************************************************************/
VOID Pdc_Rear_ProcessData()
{
	/* Store the distance measured. */
	Pdc_Rear_Distance = (3 * Tim5_CalculatedDistance_ChannelFour) / 4;
	/* Trigger the buzzer according to the distance measured. */
	if(Pdc_Rear_Distance <= PDC_MAX_DISTANCE)
	{
		Pdc_Rear_TriggerBuzzer();
	}
	else
	{
		/* do nothing */
	}

	if(Pdc_Rear_Distance >= PDC_MAX_DISTANCE)
	{
		HAL_GPIO_WritePin(PDCR_BUZZER_PORT, PDCR_BUZZER_PIN, STD_LOW);
		osTimerStop(Os_PdcR_Buzzer_TimerHandle);
		PdcR_DistanceRange = 0;
	}
	else
	{
		/* do nothing */
	}
}
/***********************************************************************************
* END OF Pdc_Rear_ProcessData											  		   * 		   																	       																	   *
************************************************************************************/
/***********************************************************************************
* Function: Pdc_Front_ProcessData												   *
* Description: Process distance data. 		   			                           *
************************************************************************************/
VOID Pdc_Front_ProcessData()
{
	/* Store the distance measured. */
	Pdc_Front_Distance = (3 * Tim5_CalculatedDistance_ChannelThree) / 4;
	/* Trigger the buzzer according to the distance measured. */
	if(Pdc_Front_Distance <= PDC_MAX_DISTANCE)
	{
		Pdc_Front_TriggerBuzzer();
	}
	else
	{
		/* do nothing */
	}

	if(Pdc_Front_Distance >= PDC_MAX_DISTANCE)
	{
		HAL_GPIO_WritePin(PDCF_BUZZER_PORT, PDCF_BUZZER_PIN, STD_LOW);
		osTimerStop(Os_PdcF_Buzzer_TimerHandle);
		PdcF_DistanceRange = 0;
	}
	else
	{
		/* do nothing */
	}
}
/***********************************************************************************
* END OF Pdc_Front_ProcessData											  		   * 		   																	       																	   *
************************************************************************************/
/***********************************************************************************
* Function: Pdc_Rear_TriggerBuzzer												   *
* Description: Control buzzer.		   			                                   *
************************************************************************************/
VOID Pdc_Rear_TriggerBuzzer()
{
	if(osTimerIsRunning(Os_PdcR_Buzzer_TimerHandle) == 0)
	{
		switch(PdcR_DistanceRange)
		{
			case 1:
				PdcR_DistanceRange = 0;
				osTimerStart(Os_PdcR_Buzzer_TimerHandle, PDC_BUZZER_PERIOD_ONE);
				break;
			case 2:
				PdcR_DistanceRange = 0;
				osTimerStart(Os_PdcR_Buzzer_TimerHandle, PDC_BUZZER_PERIOD_TWO);
				break;
			case 3:
				PdcR_DistanceRange = 0;
				osTimerStart(Os_PdcR_Buzzer_TimerHandle, PDC_BUZZER_PERIOD_THREE);
				break;
			case 4:
				PdcR_DistanceRange = 0;
				osTimerStart(Os_PdcR_Buzzer_TimerHandle, PDC_BUZZER_PERIOD_FOUR);
				break;
			default:
				break;
		}
	}
	else
	{
		/* do nothing */
	}
	/* Trigger the buzzer for each case and type of safety distance. */
	if(Pdc_Rear_Distance <= PDC_MAX_DISTANCE && Pdc_Rear_Distance >= PDC_FIRST_SAFE_DISTANCE)
	{
		PdcR_DistanceRange = 1;
	}
	else
	{
		/* do nothing */
	}

	if(Pdc_Rear_Distance <= PDC_FIRST_SAFE_DISTANCE && Pdc_Rear_Distance >= PDC_SECOND_SAFE_DISTANCE)
	{
		PdcR_DistanceRange = 2;
	}
	else
	{
		/* do nothing */
	}

	if(Pdc_Rear_Distance <= PDC_SECOND_SAFE_DISTANCE && Pdc_Rear_Distance >= PDC_THIRD_SAFE_DISTANCE)
	{
		PdcR_DistanceRange = 3;
	}
	else
	{
		/* do nothing */
	}

	if(Pdc_Rear_Distance <= PDC_THIRD_SAFE_DISTANCE)
	{
		PdcR_DistanceRange = 4;
	}
	else
	{
		/* do nothing */
	}
}
/***********************************************************************************
* END OF Pdc_Rear_TriggerBuzzer											  		   * 		   																	       																	   *
************************************************************************************/
/***********************************************************************************
* Function: Pdc_Front_TriggerBuzzer												   *
* Description: Control buzzer		   			   								   *
************************************************************************************/
VOID Pdc_Front_TriggerBuzzer()
{
	if(osTimerIsRunning(Os_PdcF_Buzzer_TimerHandle) == 0)
	{
		switch(PdcF_DistanceRange)
		{
			case 1:
				PdcF_DistanceRange = 0;
				osTimerStart(Os_PdcF_Buzzer_TimerHandle, PDC_BUZZER_PERIOD_ONE);
				break;
			case 2:
				PdcF_DistanceRange = 0;
				osTimerStart(Os_PdcF_Buzzer_TimerHandle, PDC_BUZZER_PERIOD_TWO);
				break;
			case 3:
				PdcF_DistanceRange = 0;
				osTimerStart(Os_PdcF_Buzzer_TimerHandle, PDC_BUZZER_PERIOD_THREE);
				break;
			case 4:
				PdcF_DistanceRange = 0;
				osTimerStart(Os_PdcF_Buzzer_TimerHandle, PDC_BUZZER_PERIOD_FOUR);
				break;
			default:
				break;
		}
	}
	else
	{
		/* do nothing */
	}
	/* Trigger the buzzer for each case and type of safety distance. */
	if(Pdc_Front_Distance <= PDC_MAX_DISTANCE && Pdc_Front_Distance >= PDC_FIRST_SAFE_DISTANCE)
	{
		PdcF_DistanceRange = 1;
	}
	else
	{
		/* do nothing */
	}

	if(Pdc_Front_Distance <= PDC_FIRST_SAFE_DISTANCE && Pdc_Front_Distance >= PDC_SECOND_SAFE_DISTANCE)
	{
		PdcF_DistanceRange = 2;
	}
	else
	{
		/* do nothing */
	}

	if(Pdc_Front_Distance <= PDC_SECOND_SAFE_DISTANCE && Pdc_Front_Distance >= PDC_THIRD_SAFE_DISTANCE)
	{
		PdcF_DistanceRange = 3;
	}
	else
	{
		/* do nothing */
	}

	if(Pdc_Front_Distance <= PDC_THIRD_SAFE_DISTANCE)
	{
		PdcF_DistanceRange = 4;
	}
	else
	{
		/* do nothing */
	}
}
/***********************************************************************************
* END OF Pdc_Front_TriggerBuzzer											  	   * 		   																	       																	   *
************************************************************************************/
