#include "g26_tp.h"
#include <string.h>

static void frame_init(G26_CanFrame *frame)
{
    memset(frame, 0, sizeof *frame);
    frame->id = G26_DIAG_RESPONSE_ID;
    frame->dlc = 8;
    memset(frame->data, 0xAA, 8);
    frame->data[0] = G26_TESTER_ADDRESS;
}

void G26_TpInit(G26_Tp *tp) { memset(tp, 0, sizeof *tp); }

void G26_TpTick(G26_Tp *tp, uint32_t now_ms)
{
    if (tp->rx_active && (int32_t)(now_ms - tp->rx_deadline) >= 0) tp->rx_active = 0;
    if (tp->tx_active && (int32_t)(now_ms - tp->tx_deadline) >= 0) tp->tx_active = 0;
}

int G26_TpReceive(G26_Tp *tp, const G26_CanFrame *in, uint32_t now_ms, G26_CanFrame *out)
{
    uint8_t kind, amount, status;
    uint16_t length;
    out->dlc = 0;
    G26_TpTick(tp, now_ms);
    if (in->id != G26_DIAG_REQUEST_ID || in->dlc < 2 || in->dlc > 8) return 0;
    kind = in->data[1] >> 4;
    if(kind==3 ? in->data[0]!=0x41u : in->data[0]!=G26_DIAG_ROUTE)return 0;
    if (kind == 0) {
        length = in->data[1] & 0x0Fu;
        if (!length || length > 6 || length + 2u > in->dlc) return 0;
        memcpy(tp->rx, in->data + 2, length);
        tp->rx_length = tp->rx_used = length;
        tp->rx_active = 0;
        return 1;
    }
    if (kind == 1) {
        if (in->dlc != 8) return 0;
        length = ((uint16_t)(in->data[1] & 0x0Fu) << 8) | in->data[2];
        if (length > G26_TP_MAX_PAYLOAD) {
            frame_init(out);
            out->data[1] = 0x32; /* FlowControl overflow */
            return 0;
        }
        if (length <= 6) return 0;
        tp->rx_length = length;
        tp->rx_used = 5;
        tp->rx_sn = 1;
        tp->rx_block = 0;
        tp->rx_active = 1;
        tp->rx_deadline = now_ms + G26_TP_TIMEOUT_MS;
        memcpy(tp->rx, in->data + 3, 5);
        frame_init(out);
        out->data[1] = 0x30; out->data[2] = 8; out->data[3] = 0;
        return 0;
    }
    if (kind == 2 && tp->rx_active) {
        if ((in->data[1] & 0x0Fu) != tp->rx_sn || in->dlc < 3) {
            tp->rx_active = 0;
            return 0;
        }
        amount = (uint8_t)(tp->rx_length - tp->rx_used);
        if (amount > 6) amount = 6;
        if (in->dlc < amount + 2u) { tp->rx_active = 0; return 0; }
        memcpy(tp->rx + tp->rx_used, in->data + 2, amount);
        tp->rx_used += amount;
        tp->rx_sn = (tp->rx_sn + 1u) & 0x0Fu;
        tp->rx_deadline = now_ms + G26_TP_TIMEOUT_MS;
        if (tp->rx_used == tp->rx_length) { tp->rx_active = 0; return 1; }
        if (++tp->rx_block == 8) {
            tp->rx_block = 0;
            frame_init(out);
            out->data[1] = 0x30; out->data[2] = 8; out->data[3] = 0;
        }
        return 0;
    }
    if (kind == 3 && tp->tx_active && tp->tx_wait_fc && in->dlc >= 4) {
        status = in->data[1] & 0x0Fu;
        if (status == 2) { tp->tx_active = 0; return 0; }
        if (status == 1) {
            if (++tp->tx_wait_count > 3u) tp->tx_active = 0;
            else tp->tx_deadline = now_ms + G26_TP_TIMEOUT_MS;
            return 0;
        }
        if (status != 0 ||
            (in->data[3] > 0x7Fu &&
             (in->data[3] < 0xF1u || in->data[3] > 0xF9u))) {
            tp->tx_active = 0;
            return 0;
        }
        tp->tx_block_size = in->data[2];
        tp->tx_wait_count = 0;
        /* HAL scheduling is millisecond-based. Round the ISO-TP 100-900 us
         * encodings up to 1 ms so the sender never violates minimum spacing. */
        tp->tx_stmin = in->data[3] >= 0xF1u ? 1u : in->data[3];
        tp->tx_block = 0;
        tp->tx_wait_fc = 0;
        tp->tx_next_ms = now_ms;
        tp->tx_deadline = now_ms + G26_TP_TIMEOUT_MS;
    }
    return 0;
}

int G26_TpStart(G26_Tp *tp, const uint8_t *payload, size_t length,
               uint32_t now_ms, G26_CanFrame *out)
{
    if (!payload || !length || length > G26_TP_MAX_PAYLOAD || tp->tx_active) return 0;
    frame_init(out);
    if (length <= 6) {
        out->data[1] = (uint8_t)length;
        memcpy(out->data + 2, payload, length);
        return 1;
    }
    memcpy(tp->tx, payload, length);
    tp->tx_length = (uint16_t)length;
    tp->tx_used = 5;
    tp->tx_sn = 1;
    tp->tx_wait_count = 0;
    tp->tx_active = tp->tx_wait_fc = 1;
    tp->tx_deadline = now_ms + G26_TP_TIMEOUT_MS;
    out->data[1] = 0x10u | (uint8_t)(length >> 8);
    out->data[2] = (uint8_t)length;
    memcpy(out->data + 3, payload, 5);
    return 1;
}

int G26_TpNext(G26_Tp *tp, uint32_t now_ms, G26_CanFrame *out)
{
    uint16_t amount;
    G26_TpTick(tp, now_ms);
    if (!tp->tx_active || tp->tx_wait_fc || (int32_t)(now_ms - tp->tx_next_ms) < 0)
        return 0;
    frame_init(out);
    out->data[1] = 0x20u | tp->tx_sn;
    amount = tp->tx_length - tp->tx_used;
    if (amount > 6) amount = 6;
    memcpy(out->data + 2, tp->tx + tp->tx_used, amount);
    tp->tx_used += amount;
    tp->tx_sn = (tp->tx_sn + 1u) & 0x0Fu;
    tp->tx_next_ms = now_ms + tp->tx_stmin;
    tp->tx_deadline = now_ms + G26_TP_TIMEOUT_MS;
    if (tp->tx_used == tp->tx_length) tp->tx_active = 0;
    else if (tp->tx_block_size && ++tp->tx_block == tp->tx_block_size)
        tp->tx_wait_fc = 1;
    return 1;
}
