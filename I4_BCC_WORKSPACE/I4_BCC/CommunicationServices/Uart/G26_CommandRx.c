#include "G26_CommandRx.h"
#ifndef G26_HOST_TEST
#include "main.h"
#endif
#define CAPACITY 16U
static volatile uint8_t queue[CAPACITY],head,tail;
static volatile uint32_t dropped;
static uint16_t value;
static uint8_t digits,invalid;
void G26_CommandRxInit(void){head=tail=0;dropped=0;value=0;digits=invalid=0;}
void G26_CommandRxByte(uint8_t byte){
 if(byte=='\r')return;
 if(byte=='\n'){
  if(digits && !invalid){
   uint8_t next=(uint8_t)((head+1U)%CAPACITY);
   if(next==tail){if(dropped!=UINT32_MAX)++dropped;}
   else {queue[head]=(uint8_t)value;head=next;}
  }
  value=0;digits=invalid=0;return;
 }
 if(invalid)return;
 if(byte<'0' || byte>'9' || digits==3){invalid=1;return;}
 value=(uint16_t)(value*10U+(byte-'0'));++digits;
 if(value>255)invalid=1;
}
uint8_t G26_CommandRxPop(uint8_t *command){
 if(!command)return 0;
#ifndef G26_HOST_TEST
 uint32_t irq=__get_PRIMASK();__disable_irq();
#endif
 uint8_t available=tail!=head;
 if(available){*command=queue[tail];tail=(uint8_t)((tail+1U)%CAPACITY);}
#ifndef G26_HOST_TEST
 if(!irq)__enable_irq();
#endif
 return available;
}
uint32_t G26_CommandRxDropped(void){return dropped;}
