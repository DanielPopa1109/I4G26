#include "g26_mcp2515.h"
#include "g26_board.h"
#define G26_DIAG_REQUEST_ID 0x710U
#include <string.h>

#define MCP_RESET 0xC0u
#define MCP_READ 0x03u
#define MCP_WRITE 0x02u
#define MCP_BIT_MODIFY 0x05u
#define MCP_CANSTAT 0x0Eu
#define MCP_CANCTRL 0x0Fu
#define MCP_CNF3 0x28u
#define MCP_CANINTE 0x2Bu
#define MCP_CANINTF 0x2Cu
#define MCP_EFLG 0x2Du
#define MCP_TXB0CTRL 0x30u
#define MCP_TXB0SIDH 0x31u
#define MCP_RXB0CTRL 0x60u
#define MCP_RXB1CTRL 0x70u
#define MCP_RXB0SIDH 0x61u
#define MCP_RXB1SIDH 0x71u

static int exchange(G26_Mcp2515 *d, uint8_t tx, uint8_t *rx)
{
    uint8_t discard;
    if (!rx) rx = &discard;
    if (!d->bus.exchange || d->bus.exchange(d->bus.context, tx, rx)) {
        ++d->spi_errors;
        return -1;
    }
    return 0;
}

static int command(G26_Mcp2515 *d, const uint8_t *tx, uint8_t *rx, unsigned count)
{
    unsigned i;
    uint8_t value;
    int status = 0;
    if (!d->bus.select || !d->bus.exchange) return -1;
    d->bus.select(d->bus.context, 1);
    for (i = 0; i < count; ++i) {
        if (exchange(d, tx ? tx[i] : 0, &value)) { status = -1; break; }
        if (rx) rx[i] = value;
    }
    d->bus.select(d->bus.context, 0);
    return status;
}

static int read_reg(G26_Mcp2515 *d, uint8_t reg, uint8_t *value)
{
    uint8_t tx[3] = {MCP_READ, reg, 0}, rx[3];
    if (command(d, tx, rx, 3)) return -1;
    *value = rx[2];
    return 0;
}

static int write_seq(G26_Mcp2515 *d, uint8_t reg, const uint8_t *data, unsigned length)
{
    uint8_t tx[15];
    if (length > 13) return -1;
    tx[0] = MCP_WRITE; tx[1] = reg;
    memcpy(tx + 2, data, length);
    return command(d, tx, 0, length + 2);
}

static int bit_modify(G26_Mcp2515 *d, uint8_t reg, uint8_t mask, uint8_t value)
{
    uint8_t tx[4] = {MCP_BIT_MODIFY, reg, mask, value};
    return command(d, tx, 0, 4);
}

static void standard_id(uint16_t id, uint8_t out[4])
{
    out[0] = (uint8_t)(id >> 3);
    out[1] = (uint8_t)((id & 7u) << 5);
    out[2] = 0; out[3] = 0;
}

static int set_mode(G26_Mcp2515 *d, uint8_t mode)
{
    uint8_t status;
    unsigned retry;
    if (bit_modify(d, MCP_CANCTRL, 0xE0u, mode)) return -1;
    for (retry = 0; retry < 20; ++retry) {
        if (read_reg(d, MCP_CANSTAT, &status)) return -1;
        if ((status & 0xE0u) == mode) return 0;
        if (d->bus.delay_ms) d->bus.delay_ms(d->bus.context, 1);
    }
    return -1;
}

void G26_McpInitState(G26_Mcp2515 *device, G26_McpBus bus)
{
    memset(device, 0, sizeof *device);
    device->bus = bus;
    (void)G26_McpConfigureBitrate(device,G26_MCP_OSC_HZ,500000U);
}

int G26_McpConfigureBitrate(G26_Mcp2515 *d,uint32_t oscillator,uint32_t bitrate)
{
    if(!d)return -1;
    d->timing_valid=0;
    if(bitrate!=500000U)return -1;
    /* CNF3/2/1. Rate = oscillator / (2*(BRP+1)*(1+PropSeg+PS1+PS2)). */
    if(oscillator==8000000U){d->timing[0]=0x82;d->timing[1]=0x90;d->timing[2]=0;}
    else if(oscillator==16000000U){d->timing[0]=0x82;d->timing[1]=0x90;d->timing[2]=1;}
    else if(oscillator==20000000U){d->timing[0]=3;d->timing[1]=0xBE;d->timing[2]=0;}
    else return -1;
    d->timing_valid=1;return 0;
}

static int start_with_filters(G26_Mcp2515 *d, const uint16_t ids[6])
{
    uint8_t reset = MCP_RESET;
    uint8_t filter[4], mask[4];
    uint8_t i;
    if (!d || !d->timing_valid || !d->bus.select || !d->bus.exchange || !d->bus.delay_ms) return -1;
    d->tx_pending=0;
    if (command(d, &reset, 0, 1)) return -1;
    d->bus.delay_ms(d->bus.context, 10);
    if (set_mode(d, 0x80u) || write_seq(d, MCP_CNF3, d->timing, 3)) return -1;
    standard_id(0x7FFu, mask);
    mask[1]|=0x08u; /* Compare IDE: extended frames cannot alias standard filters. */
    if (write_seq(d, 0x20u, mask, 4) || write_seq(d, 0x24u, mask, 4)) return -1;
    for (i = 0; i < 6; ++i) {
        static const uint8_t filter_regs[6] = {0x00,0x04,0x08,0x10,0x14,0x18};
        standard_id(ids[i], filter);
        if (write_seq(d, filter_regs[i], filter, 4)) return -1;
    }
    {
        uint8_t b0 = 0x04u, b1 = 0;
        uint8_t interrupts = 0x23u; /* RX0, RX1, error */
        if (write_seq(d, MCP_RXB0CTRL, &b0, 1) ||
            write_seq(d, MCP_RXB1CTRL, &b1, 1) ||
            write_seq(d, MCP_CANINTE, &interrupts, 1) ||
            bit_modify(d, MCP_CANINTF, 0xFFu, 0)) return -1;
    }
    return set_mode(d, 0);
}

int G26_McpStart(G26_Mcp2515 *d)
{
    static const uint16_t diag_ids[6] = {
        G26_DIAG_REQUEST_ID, G26_DIAG_REQUEST_ID, G26_DIAG_REQUEST_ID,
        G26_DIAG_REQUEST_ID, G26_DIAG_REQUEST_ID, G26_DIAG_REQUEST_ID
    };
    return start_with_filters(d, diag_ids);
}

int G26_McpStartApp(G26_Mcp2515 *d)
{
    static const uint16_t app_ids[6] = {
        0x040u, 0x3FFu, G26_DIAG_REQUEST_ID,
        0x040u, 0x3FFu, G26_DIAG_REQUEST_ID
    };
    return start_with_filters(d, app_ids);
}

int G26_McpSend(G26_Mcp2515 *d, const G26_CanFrame *frame)
{
    uint8_t ctrl, packet[13], rts = 0x81u;
    if (!d || d->tx_pending || !frame || frame->id > 0x7FFu || frame->dlc > 8) return -1;
    if (read_reg(d, MCP_TXB0CTRL, &ctrl) || (ctrl & 0x08u)) return -1;
    if(bit_modify(d,MCP_CANINTF,4u,0) || bit_modify(d,MCP_TXB0CTRL,0x70u,0))return -1;
    standard_id((uint16_t)frame->id, packet);
    packet[4] = frame->dlc;
    memcpy(packet + 5, frame->data, frame->dlc);
    if (write_seq(d, MCP_TXB0SIDH, packet, 5u + frame->dlc)) return -1;
    if(command(d, &rts, 0, 1))return -1;
    d->tx_pending=1;return 0;
}

void G26_McpAbortTx(G26_Mcp2515 *d)
{
    if(!d)return;
    (void)bit_modify(d,MCP_TXB0CTRL,8u,0);(void)bit_modify(d,MCP_CANINTF,4u,0);d->tx_pending=0;
}
int G26_McpTxStatus(G26_Mcp2515 *d)
{
    uint8_t ctrl,flags,error;
    if(!d || !d->tx_pending)return 0;
    if(read_reg(d,MCP_TXB0CTRL,&ctrl) || read_reg(d,MCP_CANINTF,&flags) || read_reg(d,MCP_EFLG,&error))return -1;
    d->error_passive=(error&0x18u)!=0;
    if((error&0x20u) || (ctrl&0x50u)){G26_McpAbortTx(d);return -1;}
    /* MLOA alone is arbitration loss, not failure: the controller retries. */
    if(!(ctrl&8u) && (flags&4u)){d->tx_pending=0;if(bit_modify(d,MCP_CANINTF,4u,0))return -1;return 1;}
    return 0;
}

int G26_McpPoll(G26_Mcp2515 *d, G26_CanFrame *frame)
{
    uint8_t flags, errors, reg, rx[15], tx[15] = {MCP_READ};
    unsigned i;
    if (!frame || read_reg(d, MCP_CANINTF, &flags) || read_reg(d, MCP_EFLG, &errors)) return -1;
    if (errors & 0x20u) { ++d->bus_off_count; return -1; }
    d->error_passive=(errors&0x18u)!=0;
    if (errors & 0xC0u) {
        ++d->rx_overflows;
        if (bit_modify(d, MCP_EFLG, 0xC0u, 0)) return -1;
    }
    if (!(flags & 3u)) return 0;
    reg = (flags & 1u) ? MCP_RXB0SIDH : MCP_RXB1SIDH;
    tx[1] = reg;
    if (command(d, tx, rx, 15)) return -1;
    frame->id = ((uint32_t)rx[2] << 3) | (rx[3] >> 5);
    frame->dlc = rx[6] & 0x0Fu;
    if ((rx[3] & 0x08u) || (rx[6]&0x40u) || frame->dlc > 8) {
        bit_modify(d, MCP_CANINTF, (flags & 1u) ? 1u : 2u, 0);
        return 0;
    }
    for (i = 0; i < frame->dlc; ++i) frame->data[i] = rx[7 + i];
    if (bit_modify(d, MCP_CANINTF, (flags & 1u) ? 1u : 2u, 0)) return -1;
    return 1;
}
