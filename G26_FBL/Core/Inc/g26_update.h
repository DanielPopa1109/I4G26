#ifndef G26_UPDATE_H
#define G26_UPDATE_H

#include <stddef.h>
#include <stdint.h>
#include "g26_layout.h"

/* STM32F411RET6: sectors 0-3 16 KiB, sector 4 64 KiB, sectors 5-7 128 KiB. */
#define G26_FLASH_BASE 0x08000000u
#define G26_APP_BASE   0x08020000u
#define G26_FLASH_END  G26_APP_END
#define G26_APP_VECTOR (G26_APP_BASE + 0x200u)
#define G26_IMAGE_MAGIC 0x47323631u
#define G26_MAX_BLOCK_DATA 240u

/* Header occupies the first 512 bytes of the application region. The vector
 * table begins at APP_VECTOR. Commit magic must be programmed last. */
typedef struct {
    uint32_t magic;
    uint32_t image_bytes;
    uint32_t crc32;
    uint32_t vector_address;
} G26_ImageHeader;

typedef enum {
    G26_UPDATE_OK = 0,
    G26_UPDATE_SEQUENCE,
    G26_UPDATE_RANGE,
    G26_UPDATE_IO,
    G26_UPDATE_CRC,
    G26_UPDATE_FORMAT
} G26_UpdateResult;

typedef struct {
    void *context;
    int (*erase_app)(void *context);
    int (*write)(void *context, uint32_t address, const uint8_t *data, size_t length);
    int (*read)(void *context, uint32_t address, uint8_t *data, size_t length);
} G26_Flash;

typedef struct {
    G26_Flash flash;
    uint32_t image_bytes;
    uint32_t received;
    uint32_t crc;
    uint32_t expected_crc;
    uint8_t next_block;
    uint8_t active;
    uint8_t last_block;
    uint8_t last_length;
    uint8_t last_data[G26_MAX_BLOCK_DATA];
} G26_Update;

int G26_AppRangeValid(uint32_t address, size_t length);
uint32_t G26_Crc32(uint32_t crc, const uint8_t *data, size_t length);
int G26_ImageValid(const G26_Flash *flash);
void G26_UpdateInit(G26_Update *state, G26_Flash flash);
G26_UpdateResult G26_UpdateBegin(G26_Update *state, uint32_t image_bytes, uint32_t expected_crc);
G26_UpdateResult G26_UpdateBlock(G26_Update *state, uint8_t block, const uint8_t *data, size_t length);
G26_UpdateResult G26_UpdateFinish(G26_Update *state);

#endif
