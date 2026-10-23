/* USER CODE BEGIN Header */
/*****************************************
* Project: BCM I4						 *
* Developer: Daniel Popa				 *
* Module: Real Time Operating System     *
******************************************/
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/*****************************************
*		INCLUDE PATHS					 *
******************************************/
#include "Std_Types.h"
#include "PortH.h"
#include "SystemManager.h"
#include "App_Dependencies.h"
#include "g26_app_net.h"
#include "g26_board.h"
#include "g26_bsw.h"
#include "WatchdogManager.h"
/*****************************************
*		END OF INCLUDE PATHS		     *
******************************************/
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
/*****************************************
*		VARIABLES					 	 *
******************************************/
/* OS System Timer Counter. */
unsigned long Os_Counter = STD_LOW;
/* Os counter variable for lock unlock sequence. */
uint8 Os_LockUnlockSequence_Counter = STD_LOW;
/* Os counter variable for alarm led. */
uint8 Os_TurnOnCyclic_Counter = STD_LOW;
/* Os counter variable for alarm led. */
uint8 Os_TurnOnLed_Counter = STD_LOW;
/* Os counter variable for follow me home. */
uint8 Os_FollowMeHome_Counter = STD_LOW;
/* Os counter variable for turn signal. */
uint32 Os_Lts_Counter = STD_LOW;
/* Os counter variable for turn signal. */
uint32 Os_Rts_Counter = STD_LOW;
/* Os counter variable for turn signal. */
uint32 Os_HL_Counter = STD_LOW;
/* Os counter variable for alarm sequence. */
uint8 Os_Alarm_Counter = STD_LOW;
/* Os counter variable for rear pdc counter. */
uint8 Os_Pdc_Rear_Counter = STD_LOW;
/* Os counter variable for front pdc counter. */
uint8 Os_Pdc_Front_Counter = STD_LOW;
/* Task awake. */
TickType_t ASIL_APPL_PreMain;
/* Task awake. */
TickType_t ASIL_APPL_Main;
/* Task awake. */
TickType_t ASIL_APPL_PostMain;
/* Task awake. */
TickType_t QM_APPL_PreMain;
/* Task awake. */
TickType_t QM_APPL_Main;
/* Task awake. */
TickType_t QM_APPL_PostMain;
/* Task awake. */
TickType_t ASIL_BSW_PreMain;
/* Task awake. */
TickType_t ASIL_BSW_Main;
/* Task awake. */
TickType_t ASIL_BSW_PostMain;
/* Task awake. */
TickType_t QM_BSW_PreMain;
/* Task awake. */
TickType_t QM_BSW_Main;
/* Task awake. */
TickType_t QM_BSW_PostMain;
/*****************************************
*		END OF VARIABLES				 *
******************************************/
/* USER CODE END Variables */
/* Definitions for OS_Init */
osThreadId_t OS_InitHandle;
const osThreadAttr_t OS_Init_attributes = {
  .name = "OS_Init",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityRealtime7,
};
/* Definitions for ASIL_APPL_Main */
osThreadId_t ASIL_APPL_MainHandle;
const osThreadAttr_t ASIL_APPL_Main_attributes = {
  .name = "ASIL_APPL_Main",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for QM_APPL_Main */
osThreadId_t QM_APPL_MainHandle;
const osThreadAttr_t QM_APPL_Main_attributes = {
  .name = "QM_APPL_Main",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh5,
};
/* Definitions for ASIL_BSW_Main */
osThreadId_t ASIL_BSW_MainHandle;
const osThreadAttr_t ASIL_BSW_Main_attributes = {
  .name = "ASIL_BSW_Main",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityRealtime5,
};
/* Definitions for QM_BSW_Main */
osThreadId_t QM_BSW_MainHandle;
const osThreadAttr_t QM_BSW_Main_attributes = {
  .name = "QM_BSW_Main",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityRealtime2,
};
/* Definitions for Os_SecAlm_AlarmReset */
osTimerId_t Os_SecAlm_AlarmResetHandle;
const osTimerAttr_t Os_SecAlm_AlarmReset_attributes = {
  .name = "Os_SecAlm_AlarmReset"
};
/* Definitions for Os_CenLoc_LockUnlockSequence_Timer */
osTimerId_t Os_CenLoc_LockUnlockSequence_TimerHandle;
const osTimerAttr_t Os_CenLoc_LockUnlockSequence_Timer_attributes = {
  .name = "Os_CenLoc_LockUnlockSequence_Timer"
};
/* Definitions for Os_SecAlmLed_TurnOnCyclic_Timer */
osTimerId_t Os_SecAlmLed_TurnOnCyclic_TimerHandle;
const osTimerAttr_t Os_SecAlmLed_TurnOnCyclic_Timer_attributes = {
  .name = "Os_SecAlmLed_TurnOnCyclic_Timer"
};
/* Definitions for Os_SecAlmLedTurnOn_Timer */
osTimerId_t Os_SecAlmLedTurnOn_TimerHandle;
const osTimerAttr_t Os_SecAlmLedTurnOn_Timer_attributes = {
  .name = "Os_SecAlmLedTurnOn_Timer"
};
/* Definitions for Os_FollowMeHome_Timer */
osTimerId_t Os_FollowMeHome_TimerHandle;
const osTimerAttr_t Os_FollowMeHome_Timer_attributes = {
  .name = "Os_FollowMeHome_Timer"
};
/* Definitions for Os_TurnSignals_Timer */
osTimerId_t Os_TurnSignals_TimerHandle;
const osTimerAttr_t Os_TurnSignals_Timer_attributes = {
  .name = "Os_TurnSignals_Timer"
};
/* Definitions for Os_SecAlmAlarm_Timer */
osTimerId_t Os_SecAlmAlarm_TimerHandle;
const osTimerAttr_t Os_SecAlmAlarm_Timer_attributes = {
  .name = "Os_SecAlmAlarm_Timer"
};
/* Definitions for Os_PdcR_Buzzer_Timer */
osTimerId_t Os_PdcR_Buzzer_TimerHandle;
const osTimerAttr_t Os_PdcR_Buzzer_Timer_attributes = {
  .name = "Os_PdcR_Buzzer_Timer"
};
/* Definitions for Os_PdcF_Buzzer_Timer */
osTimerId_t Os_PdcF_Buzzer_TimerHandle;
const osTimerAttr_t Os_PdcF_Buzzer_Timer_attributes = {
  .name = "Os_PdcF_Buzzer_Timer"
};
/* Definitions for Sph_ASIL_APPL */
osSemaphoreId_t Sph_ASIL_APPLHandle;
const osSemaphoreAttr_t Sph_ASIL_APPL_attributes = {
  .name = "Sph_ASIL_APPL"
};
/* Definitions for Sph_QM_APPL */
osSemaphoreId_t Sph_QM_APPLHandle;
const osSemaphoreAttr_t Sph_QM_APPL_attributes = {
  .name = "Sph_QM_APPL"
};
/* Definitions for Sph_ASIL_BSW */
osSemaphoreId_t Sph_ASIL_BSWHandle;
const osSemaphoreAttr_t Sph_ASIL_BSW_attributes = {
  .name = "Sph_ASIL_BSW"
};
/* Definitions for Sph_QM_BSW */
osSemaphoreId_t Sph_QM_BSWHandle;
const osSemaphoreAttr_t Sph_QM_BSW_attributes = {
  .name = "Sph_QM_BSW"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
/* USER CODE END FunctionPrototypes */

void OS_TASK_OS_INIT(void *argument);
void OS_TASK_ASIL_APPL_Main(void *argument);
void OS_TASK_QM_APPL_Main(void *argument);
void OS_TASK_ASIL_BSW_Main(void *argument);
void OS_TASK_QM_BSW_Main(void *argument);
void Os_SecAlm_AlarmReset_Callback(void *argument);
void Os_CenLoc_LockUnlockSequence_Timer_Callback(void *argument);
void Os_SecAlmLed_TurnOnCyclic_Timer_Callback(void *argument);
void Os_SecAlmLedTurnOn_Timer_Callback(void *argument);
void Os_FollowMeHome_Timer_Callback(void *argument);
void Os_TurnSignals_Timer_Callback(void *argument);
void Os_SecAlmAlarm_Timer_Callback(void *argument);
void Os_PdcR_Buzzer_Timer_Callback(void *argument);
void Os_PdcF_Buzzer_Timer_Callback(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationIdleHook(void);
void vApplicationTickHook(void);
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);
void vApplicationMallocFailedHook(void);

/* USER CODE BEGIN 2 */
void vApplicationIdleHook( void )
{
}
/* USER CODE END 2 */

/* USER CODE BEGIN 3 */
void vApplicationTickHook( void )
{
	Os_Counter++;
}
/* USER CODE END 3 */

/* USER CODE BEGIN 4 */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
   SystemManager_SetFault(STACK_OVERFLOW_RESET);
   SystemManager_PerformReset();
}
/* USER CODE END 4 */

/* USER CODE BEGIN 5 */
void vApplicationMallocFailedHook(void)
{
	SystemManager_SetFault(MALLOC_FAILED_RESET);
	SystemManager_PerformReset();
}
/* USER CODE END 5 */

/* USER CODE BEGIN PREPOSTSLEEP */
void PreSleepProcessing(uint32_t ulExpectedIdleTime)
{
	__disable_irq();
	Watchdog_MainFunction();
	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}

void PostSleepProcessing(uint32_t ulExpectedIdleTime)
{
	Watchdog_MainFunction();
	__enable_irq();
}
/* USER CODE END PREPOSTSLEEP */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of Sph_ASIL_APPL */
  Sph_ASIL_APPLHandle = osSemaphoreNew(1, 1, &Sph_ASIL_APPL_attributes);

  /* creation of Sph_QM_APPL */
  Sph_QM_APPLHandle = osSemaphoreNew(1, 1, &Sph_QM_APPL_attributes);

  /* creation of Sph_ASIL_BSW */
  Sph_ASIL_BSWHandle = osSemaphoreNew(1, 1, &Sph_ASIL_BSW_attributes);

  /* creation of Sph_QM_BSW */
  Sph_QM_BSWHandle = osSemaphoreNew(1, 1, &Sph_QM_BSW_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* creation of Os_SecAlm_AlarmReset */
  Os_SecAlm_AlarmResetHandle = osTimerNew(Os_SecAlm_AlarmReset_Callback, osTimerPeriodic, NULL, &Os_SecAlm_AlarmReset_attributes);

  /* creation of Os_CenLoc_LockUnlockSequence_Timer */
  Os_CenLoc_LockUnlockSequence_TimerHandle = osTimerNew(Os_CenLoc_LockUnlockSequence_Timer_Callback, osTimerPeriodic, NULL, &Os_CenLoc_LockUnlockSequence_Timer_attributes);

  /* creation of Os_SecAlmLed_TurnOnCyclic_Timer */
  Os_SecAlmLed_TurnOnCyclic_TimerHandle = osTimerNew(Os_SecAlmLed_TurnOnCyclic_Timer_Callback, osTimerPeriodic, NULL, &Os_SecAlmLed_TurnOnCyclic_Timer_attributes);

  /* creation of Os_SecAlmLedTurnOn_Timer */
  Os_SecAlmLedTurnOn_TimerHandle = osTimerNew(Os_SecAlmLedTurnOn_Timer_Callback, osTimerPeriodic, NULL, &Os_SecAlmLedTurnOn_Timer_attributes);

  /* creation of Os_FollowMeHome_Timer */
  Os_FollowMeHome_TimerHandle = osTimerNew(Os_FollowMeHome_Timer_Callback, osTimerPeriodic, NULL, &Os_FollowMeHome_Timer_attributes);

  /* creation of Os_TurnSignals_Timer */
  Os_TurnSignals_TimerHandle = osTimerNew(Os_TurnSignals_Timer_Callback, osTimerPeriodic, NULL, &Os_TurnSignals_Timer_attributes);

  /* creation of Os_SecAlmAlarm_Timer */
  Os_SecAlmAlarm_TimerHandle = osTimerNew(Os_SecAlmAlarm_Timer_Callback, osTimerPeriodic, NULL, &Os_SecAlmAlarm_Timer_attributes);

  /* creation of Os_PdcR_Buzzer_Timer */
  Os_PdcR_Buzzer_TimerHandle = osTimerNew(Os_PdcR_Buzzer_Timer_Callback, osTimerOnce, NULL, &Os_PdcR_Buzzer_Timer_attributes);

  /* creation of Os_PdcF_Buzzer_Timer */
  Os_PdcF_Buzzer_TimerHandle = osTimerNew(Os_PdcF_Buzzer_Timer_Callback, osTimerOnce, NULL, &Os_PdcF_Buzzer_Timer_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of OS_Init */
  OS_InitHandle = osThreadNew(OS_TASK_OS_INIT, NULL, &OS_Init_attributes);

  /* creation of ASIL_APPL_Main */
  ASIL_APPL_MainHandle = osThreadNew(OS_TASK_ASIL_APPL_Main, NULL, &ASIL_APPL_Main_attributes);

  /* creation of QM_APPL_Main */
  QM_APPL_MainHandle = osThreadNew(OS_TASK_QM_APPL_Main, NULL, &QM_APPL_Main_attributes);

  /* creation of ASIL_BSW_Main */
  ASIL_BSW_MainHandle = osThreadNew(OS_TASK_ASIL_BSW_Main, NULL, &ASIL_BSW_Main_attributes);

  /* creation of QM_BSW_Main */
  QM_BSW_MainHandle = osThreadNew(OS_TASK_QM_BSW_Main, NULL, &QM_BSW_Main_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_OS_TASK_OS_INIT */
/* USER CODE END Header_OS_TASK_OS_INIT */
void OS_TASK_OS_INIT(void *argument)
{
  /* USER CODE BEGIN OS_TASK_OS_INIT */
	for(;;)
	{
		EcuM_DriverInit();
		vTaskSuspend(NULL);
	}
  /* USER CODE END OS_TASK_OS_INIT */
}

/* USER CODE BEGIN Header_OS_TASK_ASIL_APPL_Main */
/* USER CODE END Header_OS_TASK_ASIL_APPL_Main */
void OS_TASK_ASIL_APPL_Main(void *argument)
{
  /* USER CODE BEGIN OS_TASK_ASIL_APPL_Main */
	for(;;)
	{
		osSemaphoreAcquire(Sph_ASIL_APPLHandle, osWaitForever);
		Btc_MainFunction();
#if G26_MCP2515_REWIRED_BOARD
		G26_AppNetApplyCommand();
#endif
		CenLoc_MainFunction();
		ExtLights_MainFunction();
		IntLights_MainFunction();
		osSemaphoreRelease(Sph_QM_APPLHandle);
		vTaskDelayUntil(&ASIL_APPL_Main, pdMS_TO_TICKS(5));
	}
  /* USER CODE END OS_TASK_ASIL_APPL_Main */
}

/* USER CODE BEGIN Header_OS_TASK_QM_APPL_Main */
/* USER CODE END Header_OS_TASK_QM_APPL_Main */
void OS_TASK_QM_APPL_Main(void *argument)
{
  /* USER CODE BEGIN OS_TASK_QM_APPL_Main */
	for(;;)
	{
		osSemaphoreAcquire(Sph_QM_APPLHandle, osWaitForever);
		SecAlm_MainFunction();
		Pdc_MainFunction();
		DiagCtrl_MainFunction();
		osSemaphoreRelease(Sph_ASIL_BSWHandle);
		vTaskDelayUntil(&QM_APPL_Main, pdMS_TO_TICKS(5));
	}
  /* USER CODE END OS_TASK_QM_APPL_Main */
}

/* USER CODE BEGIN Header_OS_TASK_ASIL_BSW_Main */
/* USER CODE END Header_OS_TASK_ASIL_BSW_Main */
void OS_TASK_ASIL_BSW_Main(void *argument)
{
  /* USER CODE BEGIN OS_TASK_ASIL_BSW_Main */
	for(;;)
	{
		osSemaphoreAcquire(Sph_ASIL_BSWHandle, osWaitForever);
		Watchdog_MainFunction();

		Uart_MainFunction();
		Crc_MainFunction();
		Tim_MainFunction();
		osSemaphoreRelease(Sph_QM_BSWHandle);
		vTaskDelayUntil(&ASIL_BSW_Main, pdMS_TO_TICKS(5));
	}
  /* USER CODE END OS_TASK_ASIL_BSW_Main */
}

/* USER CODE BEGIN Header_OS_TASK_QM_BSW_Main */
/* USER CODE END Header_OS_TASK_QM_BSW_Main */
void OS_TASK_QM_BSW_Main(void *argument)
{
  /* USER CODE BEGIN OS_TASK_QM_BSW_Main */
	for(;;)
	{
		osSemaphoreAcquire(Sph_QM_BSWHandle, osWaitForever);
		EcuM_MainFunction();
		SystemManager_MainFunction();
		Adc_MainFunction();
		G26_BswMainFunction();
		osSemaphoreRelease(Sph_ASIL_APPLHandle);
		vTaskDelayUntil(&QM_BSW_Main, pdMS_TO_TICKS(5));
	}
  /* USER CODE END OS_TASK_QM_BSW_Main */
}

/* Os_SecAlm_AlarmReset_Callback function */
void Os_SecAlm_AlarmReset_Callback(void *argument)
{
  /* USER CODE BEGIN Os_SecAlm_AlarmReset_Callback */
	/* Reset the security alarm counter trigger. */
	uint16 local = 0;
	SecAlm_SensorStatusCounter = local;
  /* USER CODE END Os_SecAlm_AlarmReset_Callback */
}

/* Os_CenLoc_LockUnlockSequence_Timer_Callback function */
void Os_CenLoc_LockUnlockSequence_Timer_Callback(void *argument)
{
  /* USER CODE BEGIN Os_CenLoc_LockUnlockSequence_Timer_Callback */
	/* If the counter variable for the lock unlock turn signals sequence is less than or equal to 6
	 * Then if there is either a lock or an unlock sequence increase the counter to process the states
	 * in the application. */
	if(Os_LockUnlockSequence_Counter <= 6
		&& ((CenLoc_CurrentState == STD_LOW && CenLoc_PreviousStateFlag == STD_HIGH)
		|| (CenLoc_CurrentState == STD_HIGH && CenLoc_PreviousStateFlag == STD_LOW)))
	{
		Os_LockUnlockSequence_Counter = Os_LockUnlockSequence_Counter + 1;
		CenLoc_BlinkCounter = Os_LockUnlockSequence_Counter;
	}
	else
	{
		/* Update the counter with out of bounds value to prevent unexpected triggering. */
		Os_LockUnlockSequence_Counter = 7;
		CenLoc_BlinkCounter = Os_LockUnlockSequence_Counter;
	}

  /* USER CODE END Os_CenLoc_LockUnlockSequence_Timer_Callback */
}

/* Os_SecAlmLed_TurnOnCyclic_Timer_Callback function */
void Os_SecAlmLed_TurnOnCyclic_Timer_Callback(void *argument)
{
  /* USER CODE BEGIN Os_SecAlmLed_TurnOnCyclic_Timer_Callback */
	/* Increase the counter variable used in the timer for the alarm LED
	 * that expires every 3,75 seconds. */
	Os_TurnOnCyclic_Counter = Os_TurnOnCyclic_Counter + 1;
	CenLoc_CyclicAlarmCounter = Os_TurnOnCyclic_Counter;
  /* USER CODE END Os_SecAlmLed_TurnOnCyclic_Timer_Callback */
}

/* Os_SecAlmLedTurnOn_Timer_Callback function */
void Os_SecAlmLedTurnOn_Timer_Callback(void *argument)
{
  /* USER CODE BEGIN Os_SecAlmLedTurnOn_Timer_Callback */
	/* Increase the counter variable used in the turning on the alarm LED for 0,25 seconds. */
	Os_TurnOnLed_Counter = Os_TurnOnLed_Counter + 1;
	CenLoc_TurnOnLedCounter = Os_TurnOnLed_Counter;
  /* USER CODE END Os_SecAlmLedTurnOn_Timer_Callback */
}

/* Os_FollowMeHome_Timer_Callback function */
void Os_FollowMeHome_Timer_Callback(void *argument)
{
  /* USER CODE BEGIN Os_FollowMeHome_Timer_Callback */
	/* Increase the counter variable used in the follow me home concept when the timer
	 * period expires for the lights to go off. */
	Os_FollowMeHome_Counter = Os_FollowMeHome_Counter + 1;
	CenLoc_FollowMeHomeCounter = Os_FollowMeHome_Counter;
  /* USER CODE END Os_FollowMeHome_Timer_Callback */
}

/* Os_TurnSignals_Timer_Callback function */
void Os_TurnSignals_Timer_Callback(void *argument)
{
  /* USER CODE BEGIN Os_TurnSignals_Timer_Callback */
	/* Increase the respective counter on request from the specific light in turn signals. */
	if(ExtLights_TurnSignalLeft_CurrentState == STD_HIGH)
	{
		Os_Lts_Counter = Os_Lts_Counter + 1;
		ExtLights_LTSFlag = Os_Lts_Counter;
	}
	else
	{
		/* do nothing */
	}

	if(ExtLights_TurnSignalRight_CurrentState == STD_HIGH)
	{
		Os_Rts_Counter = Os_Rts_Counter + 1;
		ExtLights_RTSFlag = Os_Rts_Counter;
	}
	else
	{
		/* do nothing */
	}

	if(ExtLights_HazardLight_CurrentState == STD_HIGH)
	{
		Os_HL_Counter = Os_HL_Counter + 1;
		ExtLights_HLFlag = Os_HL_Counter;
	}
	else
	{
		/* do nothing */
	}
  /* USER CODE END Os_TurnSignals_Timer_Callback */
}

/* Os_SecAlmAlarm_Timer_Callback function */
void Os_SecAlmAlarm_Timer_Callback(void *argument)
{
  /* USER CODE BEGIN Os_SecAlmAlarm_Timer_Callback */
	/* Increase the security alarm trigger counter when the alarm is on. */
	if(SecAlm_TriggerIRQCounterForTimer4 == STD_LOW)
	{
		Os_Alarm_Counter = STD_LOW;
	}
	else
	{
		/* do nothing */
	}
	Os_Alarm_Counter = Os_Alarm_Counter + 1;
	SecAlm_TriggerIRQCounterForTimer4 = Os_Alarm_Counter;
  /* USER CODE END Os_SecAlmAlarm_Timer_Callback */
}

/* Os_PdcR_Buzzer_Timer_Callback function */
void Os_PdcR_Buzzer_Timer_Callback(void *argument)
{
  /* USER CODE BEGIN Os_PdcR_Buzzer_Timer_Callback */
	HAL_GPIO_TogglePin(PDCR_BUZZER_PORT, PDCR_BUZZER_PIN);
  /* USER CODE END Os_PdcR_Buzzer_Timer_Callback */
}

/* Os_PdcF_Buzzer_Timer_Callback function */
void Os_PdcF_Buzzer_Timer_Callback(void *argument)
{
  /* USER CODE BEGIN Os_PdcF_Buzzer_Timer_Callback */
	HAL_GPIO_TogglePin(PDCF_BUZZER_PORT, PDCF_BUZZER_PIN);
  /* USER CODE END Os_PdcF_Buzzer_Timer_Callback */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/* USER CODE END Application */

