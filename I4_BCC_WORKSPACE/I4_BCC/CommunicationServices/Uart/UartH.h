/*****************************************
* Project: BCM I4						 *
* Developer: Daniel Popa				 *
* Module: Universal Asynchronous 		 *
* 		  Receiver Transmitter 			 *
* 		  Communication Protocol Handler *
******************************************/
#ifndef UartH_H
#define UartH_H
/*****************************************
*		INCLUDE PATHS					 *
******************************************/
#include "usart.h"
#include "Std_Types.h"
/*****************************************
*		END OF INCLUDE PATHS		     *
******************************************/
/*****************************************
*		VARIABLES					 	 *
******************************************/
/* Variable for the buffer size counter. */
/*****************************************
*		END OF VARIABLES				 *
******************************************/
/*****************************************
*		FUNCTIONS				 		 *
******************************************/
/* Initialization function declaration. */
EXTERN StdReturnType Uart_Init();
/* De-initialization function declaration. */
EXTERN StdReturnType Uart_DeInit();
/* Protocol main function declaration .*/
EXTERN VOID Uart_MainFunction();
/* Error register callback function declaration. */
EXTERN VOID HAL_UART_ErrorCallback(UART_HandleTypeDef *huart);
/* Received data complete callback function declaration. */
EXTERN VOID HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
/*****************************************
*		END OF FUNCTIONS				 *
******************************************/
#endif /* UartH_H */
