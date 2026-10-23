#include "g26_uds.h"
#include <string.h>
#include "Fbl_Ram.h"
#include "g26_board.h"

#define NRC_LENGTH 0x13u
#define NRC_CONDITIONS 0x22u
#define NRC_SEQUENCE 0x24u
#define NRC_RANGE 0x31u
#define NRC_PROGRAMMING 0x72u
#define NRC_WRONG_BSC 0x73u
#define NRC_SESSION 0x7Fu

static uint32_t be32(const uint8_t *p)
{
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8) | p[3];
}

static size_t negative(uint8_t sid, uint8_t nrc, uint8_t *out)
{
    out[0] = 0x7F; out[1] = sid; out[2] = nrc;
    return 3;
}

void G26_UdsInit(G26_Uds *uds, G26_Flash flash)
{
    memset(uds, 0, sizeof *uds);
    G26_UpdateInit(&uds->update, flash);
    uds->session = 1;
}

void G26_UdsTick(G26_Uds *uds, uint32_t now_ms)
{
    if (uds->session != 1 && (int32_t)(now_ms - uds->session_deadline) >= 0) {
        uds->session = 1;
        uds->erase_prepared = uds->download_active = uds->updater_requested = 0;
        uds->update.active = 0;
    }
}

size_t G26_UdsHandle(G26_Uds *uds, const uint8_t *req, size_t len,
                     uint32_t now_ms, uint8_t *out, size_t cap)
{
    uint8_t sid, sub, result;
    uint32_t address, size;
    if (!uds || !req || !len || !out || cap < 16) return 0;
    sid = req[0];
    G26_UdsTick(uds, now_ms);
    if (uds->session != 1) uds->session_deadline = now_ms + 5000u;
    if (sid == 0x10) {
        if (len != 2) return negative(sid, NRC_LENGTH, out);
        sub = req[1] & 0x7Fu;
        if (sub != 1 && sub != 2 && sub != 3) return negative(sid, NRC_RANGE, out);
        if (sub != 2) {
            uds->erase_prepared = uds->download_active = uds->updater_requested = 0;
            uds->update.active = 0;
        }
        uds->session = sub;
        uds->session_deadline = now_ms + 5000u;
        if (req[1] & 0x80u) return 0;
        out[0] = 0x50; out[1] = sub;
        out[2] = 0; out[3] = 50; out[4] = 0x01; out[5] = 0xF4;
        return 6;
    }
    if (sid == 0x3E) {
        if (len != 2 || (req[1] & 0x7Fu)) return negative(sid, NRC_LENGTH, out);
        if (req[1] & 0x80u) return 0;
        out[0] = 0x7E; out[1] = 0;
        return 2;
    }
    if (sid == 0x11) {
        if (len != 2 || req[1] != 1) return negative(sid, NRC_LENGTH, out);
        uds->reset_requested = 1;
        out[0] = 0x51; out[1] = 1;
        return 2;
    }
    if (sid == 0x31) {
        if (uds->session != 2) return negative(sid, NRC_SESSION, out);
        if(len==4 && req[1]==1 && req[2]==1 && req[3]==0x55){
#if FBL_UPDATER_ACTIVATION_ALLOWED && G26_MCP2515_REWIRED_BOARD
            if(uds->update.active || FblUpdater_Prepare())return negative(sid,NRC_CONDITIONS,out);
            uds->updater_requested=1;
            return negative(sid,0x78,out); /* Final 0x71 is emitted by SRAM after hardware takeover. */
#else
            return negative(sid,NRC_CONDITIONS,out); /* Explicit software-only deployment interlock. */
#endif
        }
        if (len != 12 || req[1] != 1 || req[2] != 0 || req[3] != 1)
            return negative(sid, NRC_RANGE, out);
        address = be32(req + 4); size = be32(req + 8);
        if (address != G26_APP_VECTOR || size < 8 || size > G26_FLASH_END - G26_APP_VECTOR)
            return negative(sid, NRC_RANGE, out);
        result = (uint8_t)G26_UpdateBegin(&uds->update, size, 0);
        if (result != G26_UPDATE_OK) return negative(sid, NRC_PROGRAMMING, out);
        uds->download_address = address;
        uds->download_size = size;
        uds->erase_prepared = 1;
        uds->download_active = 0;
        out[0] = 0x71; out[1] = 1; out[2] = 0; out[3] = 1;
        return 4;
    }
    if (sid == 0x34) {
        if (uds->session != 2) return negative(sid, NRC_SESSION, out);
        if (len != 11 || req[1] != 0 || req[2] != 0x44)
            return negative(sid, NRC_LENGTH, out);
        address = be32(req + 3); size = be32(req + 7);
        if (address != G26_APP_VECTOR || size < 8 || size > G26_FLASH_END - G26_APP_VECTOR)
            return negative(sid, NRC_RANGE, out);
        if (!uds->erase_prepared || !uds->update.active ||
            address != uds->download_address || size != uds->download_size)
            return negative(sid, NRC_SEQUENCE, out);
        uds->download_address = address;
        uds->download_size = size;
        uds->erase_prepared = 0;
        uds->download_active = 1;
        out[0] = 0x74; out[1] = 0x20; out[2] = 0; out[3] = G26_MAX_BLOCK_DATA + 2u;
        return 4;
    }
    if (sid == 0x36) {
        if (uds->session != 2) return negative(sid, NRC_SESSION, out);
        if(!uds->download_active)return negative(sid,NRC_SEQUENCE,out);
        if (len < 3 || len > G26_MAX_BLOCK_DATA + 2u) return negative(sid, NRC_LENGTH, out);
        result = (uint8_t)G26_UpdateBlock(&uds->update, req[1], req + 2, len - 2);
        if (result == G26_UPDATE_SEQUENCE) return negative(sid, NRC_WRONG_BSC, out);
        if (result == G26_UPDATE_RANGE || result == G26_UPDATE_FORMAT)
            return negative(sid, NRC_RANGE, out);
        if (result != G26_UPDATE_OK) return negative(sid, NRC_PROGRAMMING, out);
        out[0] = 0x76; out[1] = req[1];
        return 2;
    }
    if (sid == 0x37) {
        if (uds->session != 2) return negative(sid, NRC_SESSION, out);
        if(!uds->download_active)return negative(sid,NRC_SEQUENCE,out);
        if (len != 5) return negative(sid, NRC_LENGTH, out);
        uds->update.expected_crc = be32(req + 1);
        result = (uint8_t)G26_UpdateFinish(&uds->update);
        if (result == G26_UPDATE_SEQUENCE) return negative(sid, NRC_SEQUENCE, out);
        if (result != G26_UPDATE_OK) return negative(sid, NRC_PROGRAMMING, out);
        uds->download_active=0;
        out[0] = 0x77;
        return 1;
    }
    return negative(sid, 0x11, out);
}
