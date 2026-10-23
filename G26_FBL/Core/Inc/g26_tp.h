#ifndef G26_TP_H
#define G26_TP_H

#include <stdint.h>
#include <stddef.h>
#include "g26_can_types.h"

#define G26_TP_MAX_PAYLOAD 256u
#define G26_DIAG_ROUTE 0x55u
#define G26_TESTER_ADDRESS 0xF0u
#define G26_DIAG_REQUEST_ID 0x710u
#define G26_DIAG_RESPONSE_ID 0x709u
#define G26_TP_TIMEOUT_MS 1000u

typedef struct {
    uint8_t rx[G26_TP_MAX_PAYLOAD];
    uint8_t tx[G26_TP_MAX_PAYLOAD];
    uint16_t rx_length, rx_used, tx_length, tx_used;
    uint32_t rx_deadline, tx_deadline, tx_next_ms;
    uint8_t rx_sn, rx_block, rx_active, tx_sn, tx_block, tx_block_size;
    uint8_t tx_stmin, tx_active, tx_wait_fc, tx_wait_count;
} G26_Tp;

void G26_TpInit(G26_Tp *tp);
/* Return 1 when a complete UDS payload is available in tp->rx. If a flow
 * control frame must be transmitted, out->dlc is set to 8; otherwise 0. */
int G26_TpReceive(G26_Tp *tp, const G26_CanFrame *in, uint32_t now_ms, G26_CanFrame *out);
/* Start one response. First frame is returned in out. */
int G26_TpStart(G26_Tp *tp, const uint8_t *payload, size_t length,
                uint32_t now_ms, G26_CanFrame *out);
/* Return 1 when a consecutive frame is ready. */
int G26_TpNext(G26_Tp *tp, uint32_t now_ms, G26_CanFrame *out);
void G26_TpTick(G26_Tp *tp, uint32_t now_ms);

#endif
