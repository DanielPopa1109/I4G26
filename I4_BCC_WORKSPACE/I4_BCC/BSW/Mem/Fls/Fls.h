#ifndef STM32_FLS_H
#define STM32_FLS_H
#include <stdint.h>
#include "g26_layout.h"
#define FLS_BANK0 G26_NVM_BANK0
#define FLS_BANK1 G26_NVM_BANK1
#define FLS_SIZE0 G26_NVM_SIZE0
#define FLS_SIZE1 G26_NVM_SIZE1
int Fls_Read(uint32_t address, void *data, uint32_t size);
int Fls_Program(uint32_t address, const void *data, uint32_t size);
int Fls_EraseBank(uint8_t bank);
#endif
