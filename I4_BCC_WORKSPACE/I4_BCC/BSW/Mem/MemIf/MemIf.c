#include "MemIf.h"
#include "../Fee/Fee.h"

_Static_assert(MEMIF_IMAGE_SIZE == FEE_IMAGE_SIZE, "MemIf/Fee image size mismatch");
_Static_assert(MEMIF_JOB_OK == FEE_JOB_OK &&
               MEMIF_JOB_FAILED == FEE_JOB_FAILED &&
               MEMIF_JOB_PENDING == FEE_JOB_PENDING,
               "MemIf/Fee result mismatch");

/* One configured flash EEPROM device. MemIf remains the NvM device router. */
void MemIf_Init(void) { Fee_Init(); }
int MemIf_Read(uint8_t d,uint16_t b,uint16_t off,uint8_t *p,uint16_t n)
{ return Fee_Read(d,b,off,p,n); }
int MemIf_Write(uint8_t d,uint16_t b,const uint8_t *p)
{ return Fee_Write(d,b,p); }
int MemIf_InvalidateBlock(uint8_t d,uint16_t b)
{ return Fee_InvalidateBlock(d,b); }
uint8_t MemIf_GetJobResult(uint8_t d) { return Fee_GetJobResult(d); }
uint8_t MemIf_GetStatus(uint8_t d) { return Fee_GetStatus(d); }
void MemIf_MainFunction(void) { Fee_MainFunction(); }
void MemIf_SetErasePreparationCallback(uint8_t (*callback)(void))
{ Fee_SetErasePreparationCallback(callback); }
