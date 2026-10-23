#include "g26_update.h"
#include <string.h>

int G26_AppRangeValid(uint32_t address, size_t length)
{
    return address >= G26_APP_BASE && address < G26_FLASH_END &&
           length <= (size_t)(G26_FLASH_END - address);
}

uint32_t G26_Crc32(uint32_t crc, const uint8_t *data, size_t length)
{
    size_t i;
    uint8_t bit;
    for (i = 0; i < length; ++i) {
        crc ^= data[i];
        for (bit = 0; bit < 8; ++bit)
            crc = (crc >> 1) ^ ((crc & 1u) ? 0xEDB88320u : 0u);
    }
    return crc;
}

static uint32_t le32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static void put32(uint8_t *p, uint32_t value)
{
    p[0] = (uint8_t)value;
    p[1] = (uint8_t)(value >> 8);
    p[2] = (uint8_t)(value >> 16);
    p[3] = (uint8_t)(value >> 24);
}

int G26_ImageValid(const G26_Flash *flash)
{
    uint8_t header[16], vector[8], block[64];
    uint32_t length, remaining, address, crc = 0xFFFFFFFFu, stack, entry;
    size_t chunk;
    if (!flash || !flash->read || flash->read(flash->context, G26_APP_BASE, header, sizeof header))
        return 0;
    length = le32(header + 4);
    if (le32(header) != G26_IMAGE_MAGIC || le32(header + 12) != G26_APP_VECTOR ||
        length < 8 || length > G26_FLASH_END - G26_APP_VECTOR ||
        flash->read(flash->context, G26_APP_VECTOR, vector, sizeof vector))
        return 0;
    stack = le32(vector);
    entry = le32(vector + 4);
    if (stack < 0x20000000u || stack > G26_STACK_TOP || (stack & 7u) ||
        !(entry & 1u) || (entry & ~1u) < G26_APP_VECTOR ||
        (entry & ~1u) >= G26_APP_VECTOR + length)
        return 0;
    remaining = length;
    address = G26_APP_VECTOR;
    while (remaining) {
        chunk = remaining < sizeof block ? remaining : sizeof block;
        if (flash->read(flash->context, address, block, chunk)) return 0;
        crc = G26_Crc32(crc, block, chunk);
        address += (uint32_t)chunk;
        remaining -= (uint32_t)chunk;
    }
    return (crc ^ 0xFFFFFFFFu) == le32(header + 8);
}

void G26_UpdateInit(G26_Update *state, G26_Flash flash)
{
    memset(state, 0, sizeof *state);
    state->flash = flash;
}

G26_UpdateResult G26_UpdateBegin(G26_Update *state, uint32_t image_bytes, uint32_t expected_crc)
{
    if (!state || !state->flash.erase_app || !state->flash.write || !state->flash.read)
        return G26_UPDATE_IO;
    if (image_bytes < 8 || image_bytes > G26_FLASH_END - G26_APP_VECTOR)
        return G26_UPDATE_RANGE;
    state->active = 0;
    if (state->flash.erase_app(state->flash.context)) return G26_UPDATE_IO;
    state->image_bytes = image_bytes;
    state->expected_crc = expected_crc;
    state->received = 0;
    state->crc = 0xFFFFFFFFu;
    state->next_block = 1;
    state->last_length = 0;
    state->active = 1;
    return G26_UPDATE_OK;
}

G26_UpdateResult G26_UpdateBlock(G26_Update *state, uint8_t block, const uint8_t *data, size_t length)
{
    uint8_t verify[G26_MAX_BLOCK_DATA];
    uint32_t address;
    if (!state || !state->active) return G26_UPDATE_SEQUENCE;
    if (!data || !length || length > G26_MAX_BLOCK_DATA) return G26_UPDATE_FORMAT;
    if (block == state->last_block && state->received &&
        length == state->last_length && !memcmp(data, state->last_data, length))
        return G26_UPDATE_OK;
    if (block != state->next_block) return G26_UPDATE_SEQUENCE;
    if (length > state->image_bytes - state->received) return G26_UPDATE_RANGE;
    address = G26_APP_VECTOR + state->received;
    if (!G26_AppRangeValid(address, length)) return G26_UPDATE_RANGE;
    if (state->flash.write(state->flash.context, address, data, length) ||
        state->flash.read(state->flash.context, address, verify, length) ||
        memcmp(verify, data, length)) return G26_UPDATE_IO;
    state->crc = G26_Crc32(state->crc, data, length);
    state->received += (uint32_t)length;
    state->last_block = block;
    state->last_length = (uint8_t)length;
    memcpy(state->last_data, data, length);
    ++state->next_block;
    return G26_UPDATE_OK;
}

G26_UpdateResult G26_UpdateFinish(G26_Update *state)
{
    uint8_t header[16],vector[8];
    if (!state || !state->active || state->received != state->image_bytes)
        return G26_UPDATE_SEQUENCE;
    state->active = 0;
    if ((state->crc ^ 0xFFFFFFFFu) != state->expected_crc) return G26_UPDATE_CRC;
    if(state->flash.read(state->flash.context,G26_APP_VECTOR,vector,sizeof(vector)))return G26_UPDATE_IO;
    uint32_t stack=le32(vector),entry=le32(vector+4);
    if(stack<=0x20000000U || stack>G26_STACK_TOP || (stack&7U) || !(entry&1U) ||
       (entry&~1U)<G26_APP_VECTOR || (entry&~1U)>=G26_APP_VECTOR+state->image_bytes)return G26_UPDATE_FORMAT;
    /* Magic is committed separately, after all other metadata. An interrupted
     * update cannot make the FBL jump to a partially programmed image. */
    put32(header, G26_IMAGE_MAGIC);
    put32(header + 4, state->image_bytes);
    put32(header + 8, state->expected_crc);
    put32(header + 12, G26_APP_VECTOR);
    if (state->flash.write(state->flash.context, G26_APP_BASE + 4, header + 4, 12) ||
        state->flash.write(state->flash.context, G26_APP_BASE, header, 4))
        return G26_UPDATE_IO;
    return G26_ImageValid(&state->flash) ? G26_UPDATE_OK : G26_UPDATE_FORMAT;
}
