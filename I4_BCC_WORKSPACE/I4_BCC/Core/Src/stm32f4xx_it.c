/* USER CODE BEGIN Header */
/*****************************************
* Project: BCM I4						 *
* Developer: Daniel Popa				 *
* Module: Interrupt Service Routines	 *
******************************************/
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/*****************************************
*		INCLUDE PATHS					 *
******************************************/
#include "TimH.h"
#include "Pdc.h"

#include "App_Dependencies.h"
/*****************************************
*		END OF INCLUDE PATHS		     *
******************************************/
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */
/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_adc1;
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim1;

/* USER CODE BEGIN EV */
/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */
	SystemManager_SetFault(NMI_RESET);
	SystemManager_PerformReset();
  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */
	SystemManager_SetFault(HARDWARE_RESET);
	SystemManager_PerformReset();
  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */
	SystemManager_SetFault(MEMORY_FAULT_RESET);
	SystemManager_PerformReset();
  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */
	SystemManager_SetFault(BUS_FAULT_RESET);
	SystemManager_PerformReset();
  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */
	SystemManager_SetFault(USAGE_FAULT_RESET);
	SystemManager_PerformReset();
  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */
  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */
  /* USER CODE END DebugMonitor_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles PVD interrupt through EXTI line 16.
  */
void PVD_IRQHandler(void)
{
  /* USER CODE BEGIN PVD_IRQn 0 */

  /* USER CODE END PVD_IRQn 0 */
  HAL_PWR_PVD_IRQHandler();
  /* USER CODE BEGIN PVD_IRQn 1 */

  /* USER CODE END PVD_IRQn 1 */
}

/**
  * @brief This function handles Flash global interrupt.
  */
void FLASH_IRQHandler(void)
{
  /* USER CODE BEGIN FLASH_IRQn 0 */

  /* USER CODE END FLASH_IRQn 0 */
  HAL_FLASH_IRQHandler();
  /* USER CODE BEGIN FLASH_IRQn 1 */

  /* USER CODE END FLASH_IRQn 1 */
}

/**
  * @brief This function handles RCC global interrupt.
  */
void RCC_IRQHandler(void)
{
  /* USER CODE BEGIN RCC_IRQn 0 */
  /* USER CODE END RCC_IRQn 0 */
  /* USER CODE BEGIN RCC_IRQn 1 */
  /* USER CODE END RCC_IRQn 1 */
}

/**
  * @brief This function handles ADC1 global interrupt.
  */
void ADC_IRQHandler(void)
{
  /* USER CODE BEGIN ADC_IRQn 0 */
  /* USER CODE END ADC_IRQn 0 */
  HAL_ADC_IRQHandler(&hadc1);
  /* USER CODE BEGIN ADC_IRQn 1 */
  /* USER CODE END ADC_IRQn 1 */
}

/**
  * @brief This function handles TIM1 update interrupt and TIM10 global interrupt.
  */
void TIM1_UP_TIM10_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_UP_TIM10_IRQn 0 */
  /* USER CODE END TIM1_UP_TIM10_IRQn 0 */
  HAL_TIM_IRQHandler(&htim1);
  /* USER CODE BEGIN TIM1_UP_TIM10_IRQn 1 */
  /* USER CODE END TIM1_UP_TIM10_IRQn 1 */
}

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */
	/* When alarm is on turn on the LEDs at full duty cycle for the designated time. */
	if(SecAlm_PinStateChange == STD_HIGH)
	{
		TIM2->CCR1 = TIM2->ARR;
		TIM2->CCR2 = TIM2->ARR;
	}
	else
	{
		/* do nothing */
	}
	/* Upon timer interrupt generation
	 * proceed to increase the PWM duty cycle
	 * for the low beam light until 75%. */
	if(ExtLights_LowBeam_CurrentState == STD_HIGH)
	{
		if(TIM2->CCR1 < 1999)
		{
			TIM2->CCR1 = TIM2->CCR1 + 20;
		}
		else if(TIM2->CCR1 >= 1999)
		{
			TIM2->CCR1 = 1999;
		}
		else
		{
			/* do nothing */
		}
	}
	else
	{
		/* do nothing */
	}
	/* Upon timer interrupt generation
	 * proceed to increase the PWM duty cycle
	 * for the rear position light until 75%. */
	if(ExtLights_RearPositionLights_CurrentState == STD_HIGH)
	{
		if(TIM2->CCR2 < 1999)
		{
			TIM2->CCR2 = TIM2->CCR2 + 1;
		}
		else if(TIM2->CCR2 >= 1999)
		{
			TIM2->CCR2 = 1999;
		}
		else
		{
			/* do nothing */
		}
	}
	else
	{
		/* do nothing */
	}
	if(IntLights_CurrentState == STD_HIGH)
	{
		if(TIM2->CCR3 < 1999)
		{
			TIM2->CCR3 = TIM2->CCR3 + 20;
		}
		else if(TIM2->CCR3 >= 1999)
		{
			TIM2->CCR3 = 1999;
		}
		else
		{
			/* do nothing */
		}
	}
	else if(IntLights_CurrentState == STD_LOW)
	{
		if(TIM2->CCR3 > 20)
		{
			TIM2->CCR3 = TIM2->CCR3 - 20;
		}
		else if(TIM2->CCR3 <= 20)
		{
			TIM2->CCR3 = 0;
		}
		else
		{
			/* do nothing */
		}
	}
	else
	{
		/* do nothing */
	}
  /* USER CODE END TIM2_IRQn 0 */
  HAL_TIM_IRQHandler(&htim2);
  /* USER CODE BEGIN TIM2_IRQn 1 */
  /* USER CODE END TIM2_IRQn 1 */
}

/**
  * @brief This function handles TIM3 global interrupt.
  */
void TIM3_IRQHandler(void)
{
  /* USER CODE BEGIN TIM3_IRQn 0 */
	/* On central lock set to high
	 * increase the PWM duty cycle of the
	 * door led's brightness up to 100% gradually.
	 * Set it to 100% upon reaching maximum value.
	 * If the central lock is set to low, set duty cycle to 0. */
	if(CenLoc_CurrentState == STD_HIGH)
	{
		TIM3->CCR1 = TIM3->ARR;
	}
	else if(CenLoc_CurrentState == STD_LOW)
	{
		TIM3->CCR1 = 0;
	}
	else
	{
		/* do nothing */
	}
	/* On brake lights set to high
	 * increase the PWM duty cycle of the
	 * door led's brightness up to 100% gradually.
	 * Set it to 100% upon reaching maximum value.
	 * If the brake lights are set to low, set duty cycle to 0. */
	if(ExtLights_BrakeLight_CurrentState == STD_HIGH)
	{
		TIM3->CCR2 = TIM3->ARR;
	}
	else if(ExtLights_BrakeLight_CurrentState == STD_LOW)
	{
		TIM3->CCR2 = 0;
	}
	else
	{
		/* do nothing */
	}
	/* On front fog lights set to high
	 * increase the PWM duty cycle of the
	 * door led's brightness up to 100% gradually.
	 * Set it to 100% upon reaching maximum value.
	 * If the front fog lights are set to low, set duty cycle to 0. */
	if(ExtLights_FrontFogLight_CurrentState == STD_HIGH)
	{
		if(TIM3->CCR3 < 1999)
		{
			TIM3->CCR3 = TIM3->CCR3 + 20;
		}
		else if(TIM3->CCR3 >= 1999)
		{
			TIM3->CCR3 = 1999;
		}
		else
		{
			/* do nothing */
		}
	}
	else if(ExtLights_FrontFogLight_CurrentState == STD_LOW)
	{
		TIM3->CCR3 = 0;
	}
	else
	{
		/* do nothing */
	}
	/* On rear fog lights set to high
	 * increase the PWM duty cycle of the
	 * door led's brightness up to 100% gradually.
	 * Set it to 100% upon reaching maximum value.
	 * If the rear fog lights are set to low, set duty cycle to 0. */
	if(ExtLights_RearFogLight_CurrentState == STD_HIGH)
	{
		if(TIM3->CCR4 < 1999)
		{
			TIM3->CCR4 = TIM3->CCR4 + 20;
		}
		else if(TIM3->CCR4 >= 1999)
		{
			TIM3->CCR4 = 1999;
		}
		else
		{
			/* do nothing */
		}
	}
	else if(ExtLights_RearFogLight_CurrentState == STD_LOW)
	{
		TIM3->CCR4 = 0;
	}
	else
	{
		/* do nothing */
	}
  /* USER CODE END TIM3_IRQn 0 */
  HAL_TIM_IRQHandler(&htim3);
  /* USER CODE BEGIN TIM3_IRQn 1 */
  /* USER CODE END TIM3_IRQn 1 */
}

/**
  * @brief This function handles TIM4 global interrupt.
  */
void TIM4_IRQHandler(void)
{
  /* USER CODE BEGIN TIM4_IRQn 0 */
  /* USER CODE END TIM4_IRQn 0 */
  HAL_TIM_IRQHandler(&htim4);
  /* USER CODE BEGIN TIM4_IRQn 1 */
  /* USER CODE END TIM4_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */
  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles TIM5 global interrupt.
  */
void TIM5_IRQHandler(void)
{
  /* USER CODE BEGIN TIM5_IRQn 0 */
  /* USER CODE END TIM5_IRQn 0 */
  HAL_TIM_IRQHandler(&htim5);
  /* USER CODE BEGIN TIM5_IRQn 1 */
  /* USER CODE END TIM5_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream0 global interrupt.
  */
void DMA2_Stream0_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream0_IRQn 0 */

  /* USER CODE END DMA2_Stream0_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc1);
  /* USER CODE BEGIN DMA2_Stream0_IRQn 1 */

  /* USER CODE END DMA2_Stream0_IRQn 1 */
}

/**
  * @brief This function handles FPU global interrupt.
  */
void FPU_IRQHandler(void)
{
  /* USER CODE BEGIN FPU_IRQn 0 */

  /* USER CODE END FPU_IRQn 0 */
  /* USER CODE BEGIN FPU_IRQn 1 */

  /* USER CODE END FPU_IRQn 1 */
}

/* USER CODE BEGIN 1 */
/* USER CODE END 1 */
