/*****************************************
* Project: BCM I4						 *
* Developer: Daniel Popa				 *
* Module: Bluetooth Communication	     *
******************************************/
#ifndef Btc_H
#define Btc_H
/*****************************************
*		INCLUDE PATHS					 *
******************************************/
#include "Std_Types.h"
/*****************************************
*		END OF INCLUDE PATHS		     *
******************************************/
/*****************************************
*		DEFINES					 		 *
******************************************/
/* Buffer size for the UART data transmission. */
#define BTC_BUFFER_SIZE 						4
/* Bluetooth command for central lock unlocking. */
#define BTC_RX_CENLOC_ON 						1
/* Bluetooth command for central lock locking. */
#define BTC_RX_CENLOC_OFF 						2
/* Bluetooth command for high beam turn on. */
#define BTC_RX_EXTLIGHTS_HIGHBEAM_ON			3
/* Bluetooth command for high beam turn off. */
#define BTC_RX_EXTLIGHTS_HIGHBEAM_OFF 			4
/* Bluetooth command for left turn signal on. */
#define BTC_RX_EXTLIGHTS_TURNSIGNALLEFT_ON 		7
/* Bluetooth command for left turn signal off. */
#define BTC_RX_EXTLIGHTS_TURNSIGNALLEFT_OFF 	8
/* Bluetooth command for right turn signal on. */
#define BTC_RX_EXTLIGHTS_TURNSIGNALRIGHT_ON 	9
/* Bluetooth command for right turn signal off. */
#define BTC_RX_EXTLIGHTS_TURNSIGNALRIGHT_OFF 	10
/* Bluetooth command for hazard light on. */
#define BTC_RX_EXTLIGHTS_HAZARDLIGHT_ON 		11
/* Bluetooth command for hazard light off. */
#define BTC_RX_EXTLIGHTS_HAZARDLIGHT_OFF 		12
/* Bluetooth command for front fog light on. */
#define BTC_RX_EXTLIGHTS_FOGLIGHTFRONT_ON 		13
/* Bluetooth command for front fog light off. */
#define BTC_RX_EXTLIGHTS_FOGLIGHTFRONT_OFF 		14
/* Bluetooth command for rear fog light on. */
#define BTC_RX_EXTLIGHTS_FOGLIGHTREAR_ON 		15
/* Bluetooth command for rear fog light off. */
#define BTC_RX_EXTLIGHTS_FOGLIGHTREAR_OFF 		16
/* Bluetooth command for brake light on. */
#define BTC_RX_EXTLIGHTS_BRAKELIGHTON 			17
/* Bluetooth command for brake light off. */
#define BTC_RX_EXTLIGHTS_BRAKELIGHTOFF 			18
/* Bluetooth command for interior light on. */
#define BTC_RX_INTLIGHTS_INTERIORLIGHT_ON 		19
/* Bluetooth command for interior light off. */
#define BTC_RX_INTLIGHTS_INTERIORLIGHT_OFF 		20
/* Bluetooth command for reverse light on. */
#define BTC_RX_EXTLIGHTS_REVERSELIGHT_ON		21
/* Bluetooth command for reverse light off. */
#define BTC_RX_EXTLIGHTS_REVERSELIGHT_OFF		22
/* Bluetooth command for light switch position zero. */
#define BTC_RX_EXTLIGHTS_POSITIONZERO			23
/* Bluetooth command for light switch position automatic. */
#define BTC_RX_EXTLIGHTS_AUTOMATICLIGHTS		24
/* Bluetooth command for light switch position position lights. */
#define BTC_RX_EXTLIGHTS_POSITIONLIGHTS			25
/* Bluetooth command for light switch position night time lights. */
#define BTC_RX_EXTLIGHTS_NIGHTTIMELIGHTS		26
/*****************************************
* 		END OF DEFINES					 *
******************************************/
/*****************************************
*		VARIABLES					 	 *
******************************************/
/* Data buffer for bluetooth communication through UART. */
/* UART data received from bluetooth module. */
/* Counter for data length. */
/* Bluetooth command converted into uint8. */
EXTERN uint8 Btc_ReceivedDataOnBluetooth;
/* Auxiliary variable used for bluetooth command. */
EXTERN uint8 Btc_CenLoc;
/* Auxiliary variable used for bluetooth command. */
EXTERN uint8 Btc_LightSwitch;
/* Auxiliary variable used for bluetooth command. */
EXTERN uint8 Btc_HighBeam;
/* Auxiliary variable used for bluetooth command. */
EXTERN uint8 Btc_FlashHighBeam;
/* Auxiliary variable used for bluetooth command. */
EXTERN uint8 Btc_FrontFogLight;
/* Auxiliary variable used for bluetooth command. */
EXTERN uint8 Btc_TurnSignalLeft;
/* Auxiliary variable used for bluetooth command. */
EXTERN uint8 Btc_TurnSignalRight;
/* Auxiliary variable used for bluetooth command. */
EXTERN uint8 Btc_HazardLight;
/* Auxiliary variable used for bluetooth command. */
EXTERN uint8 Btc_BrakeLight;
/* Auxiliary variable used for bluetooth command. */
EXTERN uint8 Btc_RearFogLight;
/* Auxiliary variable used for bluetooth command. */
EXTERN uint8 Btc_ReverseLight;
/* Auxiliary variable used for bluetooth command. */
EXTERN uint8 Btc_IntLights;
/*****************************************
*		END OF VARIABLES				 *
******************************************/
/*****************************************
*		FUNCTIONS				 		 *
******************************************/
/* Bluetooth communication main function declaration. */
EXTERN VOID Btc_MainFunction();
/* Request command processing function declaration. */
EXTERN VOID Btc_RequestState();
/* Bluetooth command processing function declaration. */
EXTERN VOID Btc_RxVal();
/*****************************************
*		END OF FUNCTIONS				 *
******************************************/
#endif /* Btc_H */
