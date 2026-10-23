#ifndef G26_MCP2515_H
#define G26_MCP2515_H

#include "g26_can_types.h"
#include <stdint.h>

/* Byte exchange and chip select are supplied by the board-specific SPI1 HAL. */
typedef struct {
    void *context;
    void (*select)(void *context, int active);
    int (*exchange)(void *context, uint8_t tx, uint8_t *rx);
    void (*delay_ms)(void *context, uint32_t ms);
} G26_McpBus;

typedef struct {
    G26_McpBus bus;
    uint32_t spi_errors;
    uint32_t rx_overflows;
    uint32_t bus_off_count;
    uint8_t timing[3], timing_valid, tx_pending, error_passive;
} G26_Mcp2515;

void G26_McpInitState(G26_Mcp2515 *device, G26_McpBus bus);
/* CNF values and register operations adapted from F93 CBM CanSpi. */
int G26_McpStart(G26_Mcp2515 *device);
int G26_McpConfigureBitrate(G26_Mcp2515 *device,uint32_t oscillator,uint32_t bitrate);
/* 1 physical completion, 0 still pending, -1 error. TX buffer admission is separate. */
int G26_McpTxStatus(G26_Mcp2515 *device);
void G26_McpAbortTx(G26_Mcp2515 *device);
/* APP receives the ZGW body command, NM3, and routed diagnostic request.
 * FBL keeps its original diagnostic-only filter set. */
int G26_McpStartApp(G26_Mcp2515 *device);
int G26_McpSend(G26_Mcp2515 *device, const G26_CanFrame *frame);
/* 1: frame received, 0: none, -1: SPI/controller fault. */
int G26_McpPoll(G26_Mcp2515 *device, G26_CanFrame *frame);

#endif
