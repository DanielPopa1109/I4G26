/*****************************************
* Project: BCM I4						 *
* Developer: Daniel Popa				 *
* Module: Sensor Control	     		 *
******************************************/
/*****************************************
*		INCLUDE PATHS					 *
******************************************/
#include "SenCtrl.h"
#include "PortH.h"

#include "App_Dependencies.h"
/*****************************************
*		END OF INCLUDE PATHS		     *
******************************************/
/*****************************************
*		DEFINES					 		 *
******************************************/
/* Application request define. */
#define SENCTRL_EXTLIGHTS_LS_REQUEST 		0
/* Application request define. */
#define SENCTRL_SECALM_VS_REQUEST    		1
/*****************************************
* 		END OF DEFINES					 *
******************************************/
/*****************************************
*		VARIABLES					 	 *
******************************************/
/* Adc measurements array variable. */
uint32 SenCtrl_MeasuredValues[2] = {STD_LOW};
/*****************************************
*		END OF VARIABLES				 *
******************************************/
/*****************************************
*		FUNCTIONS				 		 *
******************************************/
/* Process sensor value function declaration. */
VOID SenCtrl_ProcessSensorValue(uint8 request);
/*****************************************
*		END OF FUNCTIONS				 *
******************************************/
/***********************************************************************************
* Function: SenCtrl_ProcessSensorValue									   	       *
* Description: Process the request for the sensor value. 	                       *
************************************************************************************/
VOID SenCtrl_ProcessSensorValue(uint8 request)
{
	/* Store request into local variable. */
	uint8 localRequest = request;
	/* Process sensor request. */
	switch(localRequest)
	{
		case SENCTRL_EXTLIGHTS_LS_REQUEST:
			if(SenCtrl_MeasuredValues[SENCTRL_EXTLIGHTS_LS_POSITION] >= 4000)
			{
				uint32 localls = 1;
				ExtLights_LightSensorState = localls;
			}
			else if(SenCtrl_MeasuredValues[SENCTRL_EXTLIGHTS_LS_POSITION] < 4000)
			{
				uint32 localls = 0;
				ExtLights_LightSensorState = localls;
			}
			else
			{
				/* do nothing */
			}
			break;
		case SENCTRL_SECALM_VS_REQUEST:
			if(SenCtrl_MeasuredValues[SENCTRL_SECALM_VS_REQUEST] == 4095)
			{
				uint32 localvs = STD_HIGH;
				SecAlm_SensorState = localvs;
			}
			else if(SenCtrl_MeasuredValues[SENCTRL_SECALM_VS_REQUEST] < 4095)
			{
				uint32 localvs = STD_LOW;
				SecAlm_SensorState = localvs;
			}
			else
			{
				/* do nothing */
			}
			break;
		default:
			break;
	}
}
/***********************************************************************************
* END OF SenCtrl_ProcessSensorValue										   	       * 		   																	       																	   *
************************************************************************************/
