#include "g26_flash_stm32.h"
#include "stm32f4xx_hal.h"
#include <string.h>
/* Independently contained resident backend. Busy code is copied by C startup
 * with .RamFunc/.data and never fetches instructions from Flash while BSY. */
#define FLASH_RAM __attribute__((section(".RamFunc"),noinline,optimize("Os","no-tree-loop-distribute-patterns")))
#define ERRORS (FLASH_SR_SOP|FLASH_SR_WRPERR|FLASH_SR_PGAERR|FLASH_SR_PGPERR|FLASH_SR_PGSERR)
static FLASH_RAM int wait_ready(void){
 uint32_t budget=100000000U;
 while(FLASH->SR&FLASH_SR_BSY){IWDG->KR=0xAAAAU;if(!--budget)return -1;}
 return (FLASH->SR&ERRORS)?-1:0;
}
static FLASH_RAM int unlock_flash(void){
 if(FLASH->SR&FLASH_SR_BSY)return -1;
 FLASH->SR=ERRORS|FLASH_SR_EOP;
 if(FLASH->CR&FLASH_CR_LOCK){FLASH->KEYR=0x45670123U;FLASH->KEYR=0xCDEF89ABU;}
 return (FLASH->CR&FLASH_CR_LOCK)?-1:0;
}
static FLASH_RAM void flush_caches(void){
 uint32_t acr=FLASH->ACR;
 FLASH->ACR=acr&~(FLASH_ACR_ICEN|FLASH_ACR_DCEN);
 FLASH->ACR=(acr&~(FLASH_ACR_ICEN|FLASH_ACR_DCEN))|FLASH_ACR_ICRST|FLASH_ACR_DCRST;
 FLASH->ACR=acr;__DSB();__ISB();
}
static FLASH_RAM int erase_sector(uint32_t sector){
 if(unlock_flash())return -1;
 FLASH->CR=FLASH_CR_SER|(sector<<FLASH_CR_SNB_Pos);FLASH->CR|=FLASH_CR_STRT;
 int result=wait_ready();if(!(FLASH->SR&FLASH_SR_BSY))FLASH->CR=FLASH_CR_LOCK;
 flush_caches();return result;
}
static FLASH_RAM int program_bytes(uint32_t address,const uint8_t *data,uint32_t length){
 if(unlock_flash())return -1;
 int result=0;
 for(uint32_t i=0;i<length;++i){
  uint8_t value=data[i];FLASH->CR=FLASH_CR_PG;*(volatile uint8_t *)(uintptr_t)(address+i)=value;
  if(wait_ready()){result=-1;break;}FLASH->CR=0;
 }
 if(!(FLASH->SR&FLASH_SR_BSY))FLASH->CR=FLASH_CR_LOCK;
 flush_caches();return result;
}
static int erase_app(void *opaque){
 G26_Stm32FlashContext *context=opaque;
 if(!context || !context->configured)return -1;
 for(uint32_t sector=5;sector<=6;++sector){
  uint32_t irq=__get_PRIMASK();__disable_irq();int result=erase_sector(sector);if(!irq)__enable_irq();
  if(result)return -1;
 }
 for(uint32_t at=G26_APP_BASE;at<G26_APP_END;at+=4)
  if(*(volatile uint32_t *)(uintptr_t)at!=0xFFFFFFFFU)return -1;
 return 0;
}
static int write_app(void *opaque,uint32_t address,const uint8_t *data,size_t length){
 G26_Stm32FlashContext *context=opaque;
 if(!context || !context->configured || !data || !G26_AppRangeValid(address,length))return -1;
 uint32_t irq=__get_PRIMASK();__disable_irq();int result=program_bytes(address,data,(uint32_t)length);if(!irq)__enable_irq();
 if(!result && memcmp((const void *)(uintptr_t)address,data,length))result=-1;
 return result;
}
static int read_app(void *opaque,uint32_t address,uint8_t *data,size_t length){
 (void)opaque;
 if(!data || !G26_AppRangeValid(address,length))return -1;
 IWDG->KR=0xAAAAU;memcpy(data,(const void *)(uintptr_t)address,length);return 0;
}
G26_Flash G26_Stm32FlashCreate(G26_Stm32FlashContext *context){
 G26_Flash result={context,erase_app,write_app,read_app};return result;
}
