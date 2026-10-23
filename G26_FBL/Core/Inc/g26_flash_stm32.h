#ifndef G26_FLASH_STM32_H
#define G26_FLASH_STM32_H

#include "g26_update.h"

/* x8 programming/erase; board power and recovery still require verification. */
typedef struct { uint8_t configured; } G26_Stm32FlashContext;
G26_Flash G26_Stm32FlashCreate(G26_Stm32FlashContext *context);

#endif
