/* STM32F411 NvM backend. Flash-busy code executes in SRAM; no HAL/Flash calls
 * occur after STRT/PG. Interrupt state is restored and the existing IWDG is fed.
 * Sector erase still pauses normal single-bank execution; bench timing is required. */
#include "Fls.h"
#include "main.h"
#include <string.h>
#define RAM_FLASH __attribute__((section(".RamFunc"),noinline,optimize("Os","no-tree-loop-distribute-patterns")))
#define ERRORS (FLASH_SR_SOP|FLASH_SR_WRPERR|FLASH_SR_PGAERR|FLASH_SR_PGPERR|FLASH_SR_PGSERR)
static int writable(uint32_t a,uint32_t n) {
 return n && ((a>=FLS_BANK0 && a<FLS_BANK0+FLS_SIZE0 && n<=FLS_BANK0+FLS_SIZE0-a) ||
              (a>=FLS_BANK1 && a<FLS_BANK1+FLS_SIZE1 && n<=FLS_BANK1+FLS_SIZE1-a));
}
static RAM_FLASH int waitReady(void) {
 uint32_t budget=100000000U;
 while(FLASH->SR&FLASH_SR_BSY){
  IWDG->KR=0xAAAAU;
  if(!--budget)return 1;
 }
 return (FLASH->SR&ERRORS)!=0;
}
static RAM_FLASH int unlock(void) {
 if(FLASH->SR&FLASH_SR_BSY)return 1;
 FLASH->SR=ERRORS|FLASH_SR_EOP;
 if(FLASH->CR&FLASH_CR_LOCK){FLASH->KEYR=0x45670123U;FLASH->KEYR=0xCDEF89ABU;}
 return (FLASH->CR&FLASH_CR_LOCK)!=0;
}
static RAM_FLASH void flushCaches(void) {
 uint32_t acr=FLASH->ACR;
 FLASH->ACR=acr&~(FLASH_ACR_ICEN|FLASH_ACR_DCEN);
 FLASH->ACR=(acr&~(FLASH_ACR_ICEN|FLASH_ACR_DCEN))|FLASH_ACR_ICRST|FLASH_ACR_DCRST;
 FLASH->ACR=acr;__DSB();__ISB();
}
static RAM_FLASH int eraseRam(uint32_t sector) {
 if(unlock())return 1;
 FLASH->CR=FLASH_CR_SER|(sector<<FLASH_CR_SNB_Pos);
 FLASH->CR|=FLASH_CR_STRT;
 int result=waitReady();
 if(!(FLASH->SR&FLASH_SR_BSY))FLASH->CR=FLASH_CR_LOCK;
 flushCaches();return result;
}
static RAM_FLASH int programRam(uint32_t at,const uint8_t *p,uint32_t n) {
 if(unlock())return 1;
 int result=0;
 for(uint32_t i=0;i<n;i+=4){
  uint32_t word=(uint32_t)p[i]|((uint32_t)p[i+1]<<8)|((uint32_t)p[i+2]<<16)|((uint32_t)p[i+3]<<24);
  FLASH->CR=FLASH_CR_PSIZE_1|FLASH_CR_PG; /* x32: configured 2.7..3.6 V lab supply */
  *(volatile uint32_t *)(uintptr_t)(at+i)=word;
  if(waitReady()){result=1;break;}
  FLASH->CR=0;
 }
 if(!(FLASH->SR&FLASH_SR_BSY))FLASH->CR=FLASH_CR_LOCK;
 flushCaches();return result;
}
int Fls_Read(uint32_t address,void *data,uint32_t size) {
 if(!data || address<0x08000000U || address>=0x08080000U || size>0x08080000U-address)return 1;
 IWDG->KR=0xAAAAU; /* Startup journal scans can contain many CRC-valid records. */
 memcpy(data,(const void *)(uintptr_t)address,size);return 0;
}
int Fls_Program(uint32_t address,const void *data,uint32_t size) {
 if(!data || (address&3U) || (size&3U) || !writable(address,size))return 1;
 uint32_t irq=__get_PRIMASK();__disable_irq();
 int result=programRam(address,data,size);
 if(!irq)__enable_irq();
 if(!result && memcmp((const void *)(uintptr_t)address,data,size))result=1;
 return result;
}
int Fls_EraseBank(uint8_t bank) {
 if(bank>1)return 1;
 uint32_t base=bank?FLS_BANK1:FLS_BANK0,size=bank?FLS_SIZE1:FLS_SIZE0;
 uint32_t irq=__get_PRIMASK();__disable_irq();
 int result=eraseRam(bank?7U:4U);
 if(!irq)__enable_irq();
 if(!result)for(uint32_t off=0;off<size;off+=4)
  if(*(volatile uint32_t *)(uintptr_t)(base+off)!=0xFFFFFFFFU){result=1;break;}
 return result;
}
