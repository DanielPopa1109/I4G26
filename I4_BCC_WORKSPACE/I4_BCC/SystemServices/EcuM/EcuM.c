/*****************************************
* Project: BCM I4						 *
* Developer: Daniel Popa				 *
* Module: Electronic Control Unit		 *
* 		  Manager						 *
******************************************/
/*****************************************
*		INCLUDE PATHS					 *
******************************************/
#include "EcuM.h"
#include "AdcH.h"
#include "TimH.h"
#include "PortH.h"
#include "spi.h"
#include "g26_board.h"
#include "g26_bsw.h"
#include "dma.h"
#include "CrcH.h"
#include "UartH.h"
#include "WatchdogManager.h"
#include "SystemManager.h"
#include "DiagCtrl.h"
#include "Pdc.h"
#include "App_Dependencies.h"
#include "g26_app_net.h"
/*****************************************
*		END OF INCLUDE PATHS		     *
******************************************/
/*****************************************
*		DEFINES					 		 *
******************************************/
#define ECUM_INIT_STATE				0x00
#define ECUM_DEINIT_STATE 			0x03
#define ECUM_CHECKFORWAKEUP_STATE 	0x01
#define ECUM_PROCESSWAKEUP_STATE 	0x02
/*****************************************
* 		END OF DEFINES					 *
******************************************/
/*****************************************
*		VARIABLES					 	 *
******************************************/
/* Variable to store Electronic Control Unit Manager state. */
uint8 EcuM_BswState;
/* Variable to store wake up source. */
uint8 EcuM_GlobalState;
/*****************************************
*		END OF VARIABLES				 *
******************************************/
/*****************************************
*		FUNCTIONS				 		 *
******************************************/
/* Driver initialization function declaration. */
StdReturnType EcuM_DriverInit();
/* Driver de-initialization function declaration. */
StdReturnType EcuM_DriverDeInit();
/* Electronic control unit main function declaration. */
VOID EcuM_MainFunction();
/* Electronic control unit wake up source storing function declaration. */
VOID EcuM_SetWakeupSource(uint32 wakeupSource);
/* Electronic control unit wake up source processing function declaration. */
VOID EcuM_ProcessWakeupEvent();
/* Electronic control unit wake up source check function declaration. */
VOID EcuM_CheckForWakeupEvent();
/*****************************************
*		END OF FUNCTIONS				 *
******************************************/
/***********************************************************************************
* Function: EcuM_CheckForWakeupEvent									   		   *
* Description: Checks for the wake up event and sends it forward for processing.   *
************************************************************************************/
VOID EcuM_CheckForWakeupEvent()
{
	/* Check if power-on reset wake-up event occurred. */
	if((RCC->CSR & RCC_CSR_PORRSTF) != 0)
	{
		/* Set the wake-up event. */
		EcuM_SetWakeupSource(ECUM_WAKEUPSOURCE_POR);
		/* Reset the flag. */
		RCC->CSR |= RCC_CSR_PORRSTF;
	}/* Check if brown-out reset wake-up event occurred.*/
	else if((RCC->CSR & RCC_CSR_BORRSTF) != 0)
	{
		/* Set the wake-up event. */
		EcuM_SetWakeupSource(ECUM_WAKEUPSOURCE_BOR);
		/* Reset the flag. */
		RCC->CSR |= RCC_CSR_BORRSTF;
	}/* Check if a software reset wake-up event occurred. */
	else if((RCC->CSR & RCC_CSR_SFTRSTF) != 0)
	{
		/* Set the wake-up event. */
		EcuM_SetWakeupSource(ECUM_WAKEUPSOURCE_SOFTWARE_RESET);
		/* Reset the flag. */
		RCC->CSR |= RCC_CSR_SFTRSTF;
	}/* Check if a windowed watchdog reset wake-up event occurred. */
	else if((RCC->CSR & RCC_CSR_IWDGRSTF) != 0)
	{
		/* Set the wake-up event. */
		EcuM_SetWakeupSource(ECUM_WAKEUPSOURCE_WINDOWED_WATCHDOG_RESET);
		/* Reset the flag. */
		RCC->CSR |= RCC_CSR_IWDGRSTF;
	}/* Check if a low power reset wake-up event occurred. */
	else if((RCC->CSR & RCC_CSR_LPWRRSTF) != 0)
	{
		/* Set the wake-up event. */
		EcuM_SetWakeupSource(ECUM_WAKEUPSOURCE_LOWPOWER_RESET);
		/* Reset the flag. */
		RCC->CSR |= RCC_CSR_LPWRRSTF;
	}/* Check if a button reset wake-up event occurred. */
	else
	{
		/* do nothing */
	}
	RCC->CSR |= RCC_CSR_RMVF;
}
/***********************************************************************************
* END OF EcuM_CheckForWakeupEvent											  	   *													       																	   *
************************************************************************************/
/***********************************************************************************
* Function: EcuM_SetWakeupSource									   		   	   *
* Description: Sets the wake up source received.			 		   			   *
************************************************************************************/
VOID EcuM_SetWakeupSource(uint32 wakeupSource)
{
	/* If wake-up source is valid. */
	if(wakeupSource != NULL_PTR)
	{
		/* Store the wake-up event. */
		EcuM_GlobalState = wakeupSource;
	}
	else
	{
		/* do nothing */
	}
}
/***********************************************************************************
* END OF EcuM_SetWakeupSource											  		   *													       																	   *
************************************************************************************/
/***********************************************************************************
* Function: EcuM_ProcessWakeupEvent									   		       *
* Description: Sends forward the wake up event.							 		   *
************************************************************************************/
VOID EcuM_ProcessWakeupEvent()
{
	/* Store the wake-up event into local variable. */
	uint8 localWakeupEvent = EcuM_GlobalState;
	/* Set the system fault for the respective wake-up event. */
	switch(localWakeupEvent)
	{
		case ECUM_WAKEUPSOURCE_POR:
			EcuM_GlobalState = 255;
			SystemManager_SetFault(ECUM_WAKEUPSOURCE_POR);
			break;
		case ECUM_WAKEUPSOURCE_BOR:
			EcuM_GlobalState = 255;
			SystemManager_SetFault(ECUM_WAKEUPSOURCE_BOR);
			break;
		case ECUM_WAKEUPSOURCE_SOFTWARE_RESET:
			EcuM_GlobalState = 255;
			SystemManager_SetFault(ECUM_WAKEUPSOURCE_SOFTWARE_RESET);
			break;
		case ECUM_WAKEUPSOURCE_WINDOWED_WATCHDOG_RESET:
			EcuM_GlobalState = 255;
			SystemManager_SetFault(ECUM_WAKEUPSOURCE_WINDOWED_WATCHDOG_RESET);
			break;
		case ECUM_WAKEUPSOURCE_LOWPOWER_RESET:
			EcuM_GlobalState = 255;
			SystemManager_SetFault(ECUM_WAKEUPSOURCE_LOWPOWER_RESET);
			break;
		default:
			break;
	}
}
/***********************************************************************************
* END OF EcuM_ProcessWakeupEvent											  	   *													       																	   *
************************************************************************************/
/***********************************************************************************
* Function: EcuM_DriverInit									   		               *
* Description: Initialize the drivers.			 		   						   *
************************************************************************************/
StdReturnType EcuM_DriverInit()
{
	Port_Init();
#if G26_MCP2515_REWIRED_BOARD
	MX_SPI1_Init();

#endif
	MX_DMA_Init();
	Tim_Init(TIMER_TWO);
	Tim_Init(TIMER_THREE);
	Tim_Init(TIMER_FOUR);
	Tim_Init(TIMER_FIVE);
	Adc_Init();
	Crc_Init();
	Uart_Init();
	Watchdog_Init();
	MX_NVIC_Init();
	G26_BswInit();
	EcuM_BswState = ECUM_CHECKFORWAKEUP_STATE;
	return E_OK;
}
/***********************************************************************************
* END OF EcuM_DriverInit											  			   *													       																	   *
************************************************************************************/
/***********************************************************************************
* Function: EcuM_DriverDeInit									   		           *
* Description: De-initialize the drivers.			 		                       *
************************************************************************************/
StdReturnType EcuM_DriverDeInit()
{
	Adc_DeInit();
	Tim_DeInit(TIMER_TWO);
	Tim_DeInit(TIMER_THREE);
	Tim_DeInit(TIMER_FOUR);
	Tim_DeInit(TIMER_FIVE);
	Uart_DeInit();
	Crc_DeInit();
	SystemManager_DeInit();
	return E_OK;
}
/***********************************************************************************
* END OF EcuM_DriverDeInit											  			   *													       																	   *
************************************************************************************/
/***********************************************************************************
* Function: EcuM_MainFunction									   		           *
* Description: Electronic control unit manager main function.			 		   *
************************************************************************************/
VOID EcuM_MainFunction()
{
	switch(EcuM_BswState)
	{
		case ECUM_INIT_STATE:
			EcuM_DriverInit();
			break;
		case ECUM_DEINIT_STATE:
			EcuM_DriverDeInit();
			break;
		case ECUM_CHECKFORWAKEUP_STATE:
			EcuM_CheckForWakeupEvent();
			EcuM_BswState = ECUM_PROCESSWAKEUP_STATE;
			break;
		case ECUM_PROCESSWAKEUP_STATE:
			EcuM_ProcessWakeupEvent();
			EcuM_BswState = ECUM_CHECKFORWAKEUP_STATE;
			break;
		default:
			break;
	}
}
/***********************************************************************************
* END OF EcuM_MainFunction											  			   *													       																	   *
************************************************************************************/
