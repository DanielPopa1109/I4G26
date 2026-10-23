/*****************************************
* Project: BCM I4						 *
* Developer: Daniel Popa				 *
* Module: Diagnostic Control	         *
******************************************/
/*****************************************
*		INCLUDE PATHS					 *
******************************************/
#include "DiagCtrl.h"
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
/* Request value processing variable. */
uint8 DiagCtrl_RequestValue = STD_LOW;
/* Fault value variable. */
uint8 DiagCtrl_FaultValue = STD_LOW;
/*****************************************
*		END OF VARIABLES				 *
******************************************/
/*****************************************
*		FUNCTIONS				 		 *
******************************************/
/* Main function declaration. */
VOID DiagCtrl_MainFunction();
/* Process requested data function declaration. */
VOID DiagCtrl_ProcessRequestedData();
/*****************************************
*		END OF FUNCTIONS				 *
******************************************/
/***********************************************************************************
* Function: DiagCtrl_MainFunction										   		   *
* Description: Application main function.		 		   						   *
************************************************************************************/
VOID DiagCtrl_MainFunction()
{
	DiagCtrl_ProcessRequestedData();
}
/***********************************************************************************
* END OF DiagCtrl_MainFunction										               *
************************************************************************************/
/***********************************************************************************
* Function: DiagCtrl_ProcessRequestedData										   *
* Description: Process the measured requested data.		 		   				   *
************************************************************************************/
VOID DiagCtrl_ProcessRequestedData()
{
	static uint32 LightSensor_RequestedValue = 0;
	static uint32 VibrationSensor_RequestedValue = 0;
	static uint8 BtcUart_FaultValue_One = 0;
	static uint8 BtcUart_FaultValue_Two = 0;
	static uint8 BtcUart_FaultValue_Three = 0;
	static uint8 BtcUart_FaultValue_Four = 0;
	LightSensor_RequestedValue = Adc_ChannelOne_Buffer[(0)];
	VibrationSensor_RequestedValue = Adc_ChannelOne_Buffer[(1)];
	BtcUart_FaultValue_One = SystemManager_Fault[(17)];
	BtcUart_FaultValue_Two = SystemManager_Fault[(18)];
	BtcUart_FaultValue_Three = SystemManager_Fault[(19)];
	BtcUart_FaultValue_Four = SystemManager_Fault[(20)];
	(void)Dem_SetEventStatus(DIAGCTRL_BLUETOOTH_MODULE_MALFUNCTION,
        (BtcUart_FaultValue_One || BtcUart_FaultValue_Two || BtcUart_FaultValue_Three || BtcUart_FaultValue_Four)?DEM_EVENT_STATUS_PREFAILED:DEM_EVENT_STATUS_PREPASSED);
    (void)Dem_SetEventStatus(DIAGCTRL_LIGHT_SENSOR_MALFUNCTION,LightSensor_RequestedValue<100?DEM_EVENT_STATUS_PREFAILED:DEM_EVENT_STATUS_PREPASSED);
    (void)Dem_SetEventStatus(DIAGCTRL_VIBRATION_SENSOR_MALFUNCTION,VibrationSensor_RequestedValue<100?DEM_EVENT_STATUS_PREFAILED:DEM_EVENT_STATUS_PREPASSED);
    (void)Dem_SetEventStatus(DIAGCTRL_FRONT_PARKING_DISTANCE_SENSOR_MALFUNCTION,
        (Tim5_CalculatedDistance_ChannelThree==0 && ExtLights_ReverseLight_CurrentState!=STD_LOW)?DEM_EVENT_STATUS_PREFAILED:DEM_EVENT_STATUS_PREPASSED);
    (void)Dem_SetEventStatus(DIAGCTRL_REAR_PARKING_DISTANCE_SENSOR_MALFUNCTION,
        (Tim5_CalculatedDistance_ChannelFour==0 && ExtLights_ReverseLight_CurrentState!=STD_LOW)?DEM_EVENT_STATUS_PREFAILED:DEM_EVENT_STATUS_PREPASSED);
}
/***********************************************************************************
* END OF DiagCtrl_ProcessRequestedData										       *
************************************************************************************/
