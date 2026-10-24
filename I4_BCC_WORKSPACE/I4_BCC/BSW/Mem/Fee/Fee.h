#ifndef FEE_H
#define FEE_H
#include <stdint.h>

/* G26's one-device EEPROM emulation: versioned, CRC-checked flash journal. */
#define FEE_IMAGE_SIZE 1024U
#define FEE_JOB_OK 0U
#define FEE_JOB_FAILED 1U
#define FEE_JOB_PENDING 2U
#define FEE_BLOCK_COUNT 1U

typedef struct { uint16_t length,version; } Fee_BlockDescriptorType;
extern const Fee_BlockDescriptorType Fee_BlockDescriptor[FEE_BLOCK_COUNT];

void Fee_Init(void);
int Fee_Read(uint8_t device,uint16_t block,uint16_t offset,uint8_t *data,uint16_t length);
int Fee_Write(uint8_t device,uint16_t block,const uint8_t *data);
int Fee_InvalidateBlock(uint8_t device,uint16_t block);
uint8_t Fee_GetJobResult(uint8_t device);
uint8_t Fee_GetStatus(uint8_t device);
void Fee_MainFunction(void);
void Fee_SetErasePreparationCallback(uint8_t (*callback)(void));
#endif
