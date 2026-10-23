/*****************************************
* Project: BCM I4						 *
* Developer: Daniel Popa				 *
* Module: Port Handler 		 			 *
******************************************/
#ifndef PortH_H
#define PortH_H
/*****************************************
*		INCLUDE PATHS					 *
******************************************/
#include "Std_Types.h"
#include "gpio.h"
#include "g26_board.h"
/*****************************************
*		END OF INCLUDE PATHS		     *
******************************************/
/*****************************************
*		DEFINES					 		 *
******************************************/
/* Port define value */
#define CENLOC_DOOR_LED_PORT 					(G26_MCP2515_REWIRED_BOARD ? GPIOC : GPIOA)
/* Pin define value */
#define CENLOC_DOOR_LED_PIN 					(G26_MCP2515_REWIRED_BOARD ? GPIO_PIN_11 : GPIO_PIN_6)
/* Port define value */
#define CENLOC_BUZZER_PORT 						GPIOB
/* Pin define value */
#define CENLOC_BUZZER_PIN						GPIO_PIN_2
/* Port define value */
#define EXTLIGHTS_LOWBEAM_PORT 					(G26_MCP2515_REWIRED_BOARD ? GPIOC : GPIOA)
/* Pin define value */
#define EXTLIGHTS_LOWBEAM_PIN 					(G26_MCP2515_REWIRED_BOARD ? GPIO_PIN_10 : GPIO_PIN_5)
/* Port define value */
#define EXTLIGHTS_HIGHBEAM_PORT 				GPIOA
/* Pin define value */
#define EXTLIGHTS_HIGHBEAM_PIN 					GPIO_PIN_12
/* Port define value */
#define EXTLIGHTS_FRONTFOGLIGHT_PORT 			GPIOB
/* Pin define value */
#define EXTLIGHTS_FRONTFOGLIGHT_PIN 			GPIO_PIN_0
/* Port define value */
#define EXTLIGHTS_TURNSIGNALLEFT_PORT 			GPIOB
/* Pin define value */
#define EXTLIGHTS_TURNSIGNALLEFT_PIN 			GPIO_PIN_6
/* Port define value */
#define EXTLIGHTS_TURNSIGNALRIGHT_PORT 			GPIOB
/* Pin define value */
#define EXTLIGHTS_TURNSIGNALRIGHT_PIN 			GPIO_PIN_7
/* Port define value */
#define EXTLIGHTS_REARPOSITIONLIGHT_PORT 		GPIOB
/* Pin define value */
#define EXTLIGHTS_REARPOSITIONLIGHT_PIN 		GPIO_PIN_10
/* Port define value */
#define EXTLIGHTS_REARFOGLIGHT_PORT 			GPIOB
/* Pin define value */
#define EXTLIGHTS_REARFOGLIGHT_PIN 				GPIO_PIN_1
/* Port define value */
#define EXTLIGHTS_BRAKELIGHT_PORT 				(G26_MCP2515_REWIRED_BOARD ? GPIOC : GPIOA)
/* Pin define value */
#define EXTLIGHTS_BRAKELIGHT_PIN 				(G26_MCP2515_REWIRED_BOARD ? GPIO_PIN_12 : GPIO_PIN_7)
/* Port define value */
#define EXTLIGHTS_REVERSELIGHT_PORT 			GPIOB
/* Pin define value */
#define EXTLIGHTS_REVERSELIGHT_PIN 				GPIO_PIN_15
/* Port define value */
#define SECALM_BUZZER_PORT 						GPIOB
/* Pin define value */
#define SECALM_BUZZER_PIN 						GPIO_PIN_12
/* Port define value */
#define SECALM_LED_PORT 						GPIOB
/* Pin define value */
#define SECALM_LED_PIN 							GPIO_PIN_14
/* Port define value */
#define PDCR_TRIG_PORT							GPIOB
/* Pin define value */
#define PDCR_TRIG_PIN							GPIO_PIN_13
/* Port define value */
#define PDCF_TRIG_PORT							GPIOA
/* Pin define value */
#define PDCF_TRIG_PIN							GPIO_PIN_11
/* Port define value */
#define PDCR_BUZZER_PORT						GPIOB
/* Pin define value */
#define PDCR_BUZZER_PIN							GPIO_PIN_5
/* Port define value */
#define PDCF_BUZZER_PORT						GPIOC
/* Pin define value */
#define PDCF_BUZZER_PIN							GPIO_PIN_13
/*****************************************
* 		END OF DEFINES					 *
******************************************/
/*****************************************
*		FUNCTIONS				 		 *
******************************************/
/* Initialization function declaration. */
EXTERN StdReturnType Port_Init();
/* De-initialization function declaration. */
EXTERN StdReturnType Port_DeInit();
/*****************************************
*		END OF FUNCTIONS				 *
******************************************/
#endif /* PortH_H */
