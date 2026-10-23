#include "g26_mcp_stm32.h"
#include "spi.h"

static void select_chip(void *context, int active)
{
    (void)context;
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, active ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

static int exchange_byte(void *context, uint8_t tx, uint8_t *rx)
{
    (void)context;
    return HAL_SPI_TransmitReceive(&hspi1, &tx, rx, 1, 10) == HAL_OK ? 0 : -1;
}

static void delay_ms(void *context, uint32_t ms)
{
    (void)context;
    HAL_Delay(ms);
}

G26_McpBus G26_McpStm32Bus(void)
{
    G26_McpBus bus = {0};
    bus.select = select_chip;
    bus.exchange = exchange_byte;
    bus.delay_ms = delay_ms;
    return bus;
}
