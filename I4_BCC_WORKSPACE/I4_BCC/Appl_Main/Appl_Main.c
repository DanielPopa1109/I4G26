/*****************************************
* Project: BCM I4						 *
* Developer: Daniel Popa				 *
* Module: Application Main				 *
******************************************/
/*****************************************
*		INCLUDE PATHS					 *
******************************************/
#include "main.h"
/*****************************************
		END OF INCLUDE PATHS		     *
******************************************/
/***********************************************************************************
* Function: main														   		   *
* Description: Defined in the linker file as the start of the application.         *
************************************************************************************/
int main()
{
	/* FBL hands over with IRQs masked until application C startup completes. */
	__enable_irq();
	/* Calls for system initialization. */
	SystemManager_Init();
	/* Initialize the kernel of FreeRTOS. */
	osKernelInitialize();
	/* Initialize the FreeRTOS with all its objects.*/
	MX_FREERTOS_Init();
	/* Start the kernel and the scheduler and FreeRTOS takes
	 * control of the application execution. */
	osKernelStart();
}
/***********************************************************************************
* END OF main											  						   * 		   																	      																	   *
************************************************************************************/
