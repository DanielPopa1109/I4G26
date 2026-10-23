/*****************************************
* Project: BCM I4						 *
* Developer: Daniel Popa				 *
* Module: Cyclic Redundancy Check		 *
* 		  Handler						 *
******************************************/
#ifndef CrcH_H
#define CrcH_H
/*****************************************
*		INCLUDE PATHS					 *
******************************************/
#include "Std_Types.h"
#include "../../Core/Inc/crc.h"
#include "../../Core/Inc/crc.h"
/*****************************************
*		END OF INCLUDE PATHS		     *
******************************************/
/*****************************************
*		VARIABLES					 	 *
******************************************/
/* Variable to store the CRC state. */
EXTERN uint8 Crc_BswState;
/* Variable to store crc calculated value. */
EXTERN uint32 Crc_CenLoc_On;
/* Variable to store crc calculated value. */
EXTERN uint32 Crc_CenLoc_Off;
/* Variable to store crc calculated value. */
EXTERN uint32 Crc_ExtLights_HighBeam_On;
/* Variable to store crc calculated value. */
EXTERN uint32 Crc_ExtLights_HighBeam_Off;
/* Variable to store crc calculated value. */
EXTERN uint32 Crc_ExtLights_FlashHighBeam_On;
/* Variable to store crc calculated value. */
EXTERN uint32 Crc_ExtLights_FlashHighBeam_Off;
/* Variable to store crc calculated value. */
EXTERN uint32 Crc_ExtLights_TurnSignalLeft_On;
/* Variable to store crc calculated value. */
EXTERN uint32 Crc_ExtLights_TurnSignalLeft_Off;
/* Variable to store crc calculated value. */
EXTERN uint32 Crc_ExtLights_TurnSignalRight_On;
/* Variable to store crc calculated value. */
EXTERN uint32 Crc_ExtLights_TurnSignalRight_Off;
/* Variable to store crc calculated value. */
EXTERN uint32 Crc_ExtLights_HazardLight_On;
/* Variable to store crc calculated value. */
EXTERN uint32 Crc_ExtLights_HazardLight_Off;
/* Variable to store crc calculated value. */
EXTERN uint32 Crc_ExtLights_FogLightFront_On;
/* Variable to store crc calculated value. */
EXTERN uint32 Crc_ExtLights_FogLightFront_Off;
/* Variable to store crc calculated value. */
EXTERN uint32 Crc_ExtLights_BrakeLight_On;
/* Variable to store crc calculated value. */
EXTERN uint32 Crc_ExtLights_BrakeLight_Off;
/* Variable to store crc calculated value. */
EXTERN uint32 Crc_ExtLights_ReverseLight_On;
/* Variable to store crc calculated value. */
EXTERN uint32 Crc_ExtLights_ReverseLight_Off;
/* Variable to store crc calculated value. */
EXTERN uint32 Crc_ExtLights_PositionZero;
/* Variable to store crc calculated value. */
EXTERN uint32 Crc_ExtLights_AutomaticLights;
/* Variable to store crc calculated value. */
EXTERN uint32 Crc_ExtLights_PositionLights;
/* Variable to store crc calculated value. */
EXTERN uint32 Crc_ExtLights_NightTimeLights;
/* Variable to store crc calculated value. */
EXTERN uint32 Crc_IntLights_InteriorLight_On;
/* Variable to store crc calculated value. */
EXTERN uint32 Crc_IntLights_InteriorLights_Off;
/* Variable to store crc calculated value. */
EXTERN uint32 Crc_ExtLights_FogLightRear_On;
/* Variable to store crc calculated value. */
EXTERN uint32 Crc_ExtLights_FogLightRear_Off;
/*****************************************
*		END OF VARIABLES				 *
******************************************/
/*****************************************
*		FUNCTIONS				 		 *
******************************************/
/* CRC initialization function declaration. */
EXTERN StdReturnType Crc_Init();
/* CRC de-initialization function declaration. */
EXTERN StdReturnType Crc_DeInit();
/* CRC UART data check function declaration. */
EXTERN StdReturnType Crc_VerifyUartData();
/* CRC main function declaration. */
EXTERN VOID Crc_MainFunction();
/* CRC error callback function declaration. */
EXTERN VOID HAL_CRC_ErrorCallback(CRC_HandleTypeDef *hcrc);
/* CRC UART data initialization for CRC check. */
EXTERN VOID Crc_InitCrcValuesForUart();
/*****************************************
*		END OF FUNCTIONS				 *
******************************************/
#endif /* CrcH_H */
