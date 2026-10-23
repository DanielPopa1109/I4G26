#include "main.h"
#include "stm32f4xx_it.h"

extern TIM_HandleTypeDef htim5;

void NMI_Handler(void) { for (;;) {} }
void HardFault_Handler(void) { for (;;) {} }
void MemManage_Handler(void) { for (;;) {} }
void BusFault_Handler(void) { for (;;) {} }
void UsageFault_Handler(void) { for (;;) {} }
void SVC_Handler(void) {}
void DebugMon_Handler(void) {}
void PendSV_Handler(void) {}
void SysTick_Handler(void) { HAL_IncTick(); }
void TIM5_IRQHandler(void) { HAL_TIM_IRQHandler(&htim5); }
