#include "main.h"
#include "gpio.h"
#include "spi.h"
#include "g26_flash_stm32.h"
#include "g26_mcp_stm32.h"
#include "g26_mcp2515.h"
#include "g26_tp.h"
#include "g26_uds.h"
#include "g26_update.h"
#include "g26_board.h"
#include "Fbl_Updater.h"

__attribute__((section(".resident_tag"),used))
const uint32_t resident_tag[8]={RAM_IMAGE_MAGIC,1,RAM_TARGET,RAM_MCU_ID,FBL_RESIDENT_SIZE,FBL_APP_START,FBL_METADATA,RAM_RESIDENT_VECTOR_WORDS};

/* The FBL is linked in sectors 0-3. Its flash adapter erases only sectors 5-6. */
static G26_Mcp2515 can_controller;
static G26_Tp transport;
static G26_Uds diagnostics;
static G26_Stm32FlashContext flash_context = {1};
static G26_Flash flash_interface;

/* Switch stacks in assembly so the compiler never accesses the old stack
 * after MSP changes. The entry address retains the Thumb bit. */
__attribute__((naked, noreturn)) static void branch_to_app(uint32_t stack,
                                                           uint32_t entry)
{
    __asm volatile ("msr msp, r0\n\tbx r1");
}

static int send_frame(const G26_CanFrame *frame)
{
    uint32_t deadline = HAL_GetTick() + 100u;
    do {
        if (G26_McpSend(&can_controller, frame) == 0) {
            do {
                int status=G26_McpTxStatus(&can_controller);
                if(status>0)return 0;
                if(status<0)return -1;
                HAL_Delay(1);
            }while((int32_t)(HAL_GetTick()-deadline)<0);
            G26_McpAbortTx(&can_controller);return -1;
        }
        HAL_Delay(1);
    } while ((int32_t)(HAL_GetTick() - deadline) < 0);
    return -1;
}

static void jump_to_application(void)
{
    uint32_t stack = *(volatile uint32_t *)G26_APP_VECTOR;
    uint32_t entry = *(volatile uint32_t *)(G26_APP_VECTOR + 4u);
    uint32_t bank;
    __disable_irq();
    HAL_SPI_DeInit(&hspi1);
    HAL_DeInit();
    for (bank = 0; bank < 3u; ++bank) {
        NVIC->ICER[bank] = 0xFFFFFFFFu;
        NVIC->ICPR[bank] = 0xFFFFFFFFu;
    }
    SysTick->CTRL = 0u;
    SysTick->LOAD = 0u;
    SysTick->VAL = 0u;
    SCB->VTOR = G26_APP_VECTOR;
    __DSB();
    __ISB();
    /* The application enables interrupts after C startup in its main(). */
    branch_to_app(stack, entry);
}

int main(void)
{
    uint8_t response[G26_TP_MAX_PAYLOAD];
    G26_CanFrame received, outgoing;
    uint32_t boot_deadline;
    size_t length;
    size_t response_length = 0;
    int can_ready;
    int polled;
    HAL_Init();
    SystemClock_Config();
#if G26_MCP2515_REWIRED_BOARD
    MX_GPIO_Init();MX_SPI1_Init();
#endif
    flash_interface = G26_Stm32FlashCreate(&flash_context);
    G26_McpInitState(&can_controller, G26_McpStm32Bus());
    G26_TpInit(&transport);
    G26_UdsInit(&diagnostics, flash_interface);
    can_ready = 0;
#if G26_MCP2515_REWIRED_BOARD
    can_ready = G26_McpStart(&can_controller) == 0;
#endif
    volatile uint32_t *box=(volatile uint32_t *)FBL_BOOT_MAILBOX;
    if(box[0]==G26_BOOT_MAGIC && box[1]==2 && box[2]==~2U){
        diagnostics.session=2;diagnostics.session_deadline=HAL_GetTick()+5000U;
    }
    box[0]=box[1]=box[2]=0;
    boot_deadline = HAL_GetTick() + 3000u;

    for (;;) {
        IWDG->KR=0xAAAAU;
        uint32_t now = HAL_GetTick();
        G26_TpTick(&transport, now);
        G26_UdsTick(&diagnostics, now);
#if G26_MCP2515_REWIRED_BOARD
        if (!can_ready && (now % 1000u) == 0u)
            can_ready = G26_McpStart(&can_controller) == 0;
#endif
        if (can_ready) {
            polled = G26_McpPoll(&can_controller, &received);
            if (polled < 0) {
                can_ready = 0;
            } else if (polled > 0) {
                int complete = G26_TpReceive(&transport, &received, now, &outgoing);
                if (outgoing.dlc && send_frame(&outgoing)) can_ready = 0;
                if (complete && response_length == 0u) {
                    /* Retain the FBL after a diagnostic request. */
                    boot_deadline = now + 3000u;
                    int dispatch_allowed = 1;
                    if (G26_UdsNeedsConfirmedPending(transport.rx,
                            transport.rx_length, diagnostics.session)) {
                        uint8_t pending[] = {0x7Fu, transport.rx[0], 0x78u};
                        if (!G26_TpStart(&transport, pending, sizeof pending,
                                         HAL_GetTick(), &outgoing) ||
                            send_frame(&outgoing)) {
                            can_ready = 0;
                            dispatch_allowed = 0;
                        } else G26_UdsConfirmPending(&diagnostics, transport.rx[0]);
                    }
                    length = dispatch_allowed ? G26_UdsHandle(&diagnostics,
                            transport.rx, transport.rx_length, now,
                            response, sizeof response) : 0u;
                    /* Flash erase is blocking. Refresh S3 using actual elapsed
                     * time so a successful erase cannot expire the session. */
                    if (diagnostics.session != 1u)
                        diagnostics.session_deadline = HAL_GetTick() + 5000u;
                    response_length = length;
                }
            }
            if (can_ready && G26_TpNext(&transport, HAL_GetTick(), &outgoing))
                if (send_frame(&outgoing)) can_ready = 0;
            if (can_ready && response_length && !transport.tx_active &&
                G26_TpStart(&transport, response, response_length,
                            HAL_GetTick(), &outgoing)) {
                if (send_frame(&outgoing)) can_ready = 0;
                else {
                    response_length = 0u;
                    if(diagnostics.updater_requested){
                        diagnostics.updater_requested=0;
                        (void)FblUpdater_Activate();
                        response[0]=0x7F;response[1]=0x31;response[2]=0x22;response_length=3;
                    }
                }
            }
        }
        if (diagnostics.reset_requested && can_ready &&
            response_length == 0u && !transport.tx_active) {
            HAL_Delay(20);
            NVIC_SystemReset();
        }
        if (!diagnostics.update.active && response_length == 0u &&
            diagnostics.session == 1u &&
            (int32_t)(now - boot_deadline) >= 0 &&
            G26_ImageValid(&flash_interface))
            jump_to_application();
        HAL_Delay(1);
    }
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef osc = {0};
    RCC_ClkInitTypeDef clock = {0};
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    osc.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    osc.HSEState = RCC_HSE_ON;
    osc.PLL.PLLState = RCC_PLL_ON;
    osc.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    osc.PLL.PLLM = 4;
    osc.PLL.PLLN = 100;
    osc.PLL.PLLP = RCC_PLLP_DIV2;
    osc.PLL.PLLQ = 4;
    if (HAL_RCC_OscConfig(&osc) != HAL_OK) Error_Handler();
    clock.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                      RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    clock.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    clock.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clock.APB1CLKDivider = RCC_HCLK_DIV2;
    clock.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&clock, FLASH_LATENCY_3) != HAL_OK) Error_Handler();
}

void Error_Handler(void)
{
    __disable_irq();
    for (;;) {}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *timer)
{
    if (timer->Instance == TIM5) HAL_IncTick();
}
