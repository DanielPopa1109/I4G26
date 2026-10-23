#ifndef G26_MCP_STM32_H
#define G26_MCP_STM32_H

#include "g26_mcp2515.h"

/* SPI1 PA5/PA6/PA7 and active-low MCP2515 CS on PC4. */
G26_McpBus G26_McpStm32Bus(void);

#endif
