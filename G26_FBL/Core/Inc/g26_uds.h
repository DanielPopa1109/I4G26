#ifndef G26_UDS_H
#define G26_UDS_H

#include "g26_update.h"
#include <stdint.h>
#include <stddef.h>

typedef struct {
    G26_Update update;
    uint32_t session_deadline;
    uint32_t download_address;
    uint32_t download_size;
    uint8_t session;
    uint8_t erase_prepared;
    uint8_t reset_requested;
    uint8_t updater_requested, download_active;
} G26_Uds;

void G26_UdsInit(G26_Uds *uds, G26_Flash flash);
/* Response capacity must be at least 16. Return response length. Reset is
 * only requested as a flag; the caller must send the response before reset. */
size_t G26_UdsHandle(G26_Uds *uds, const uint8_t *request, size_t length,
                     uint32_t now_ms, uint8_t *response, size_t capacity);
void G26_UdsTick(G26_Uds *uds, uint32_t now_ms);

#endif
