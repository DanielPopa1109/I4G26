#include "G26_CommandRx.h"
#include <assert.h>
#ifdef NDEBUG
#error Assertions must be enabled
#endif
#include <stdio.h>
static void feed(const char *s){while(*s)G26_CommandRxByte((unsigned char)*s++);}
int main(void){
 uint8_t value;G26_CommandRxInit();
 feed("1\n255\r\n");assert(G26_CommandRxPop(&value) && value==1);assert(G26_CommandRxPop(&value) && value==255);assert(!G26_CommandRxPop(&value));
 feed("\n256\n99999999999999999\nabc\n-1\n12x\n2\n");assert(G26_CommandRxPop(&value) && value==2);assert(!G26_CommandRxPop(&value));
 for(unsigned i=0;i<20;++i)feed("99\n");assert(G26_CommandRxDropped()==5);
 for(unsigned i=0;i<15;++i)assert(G26_CommandRxPop(&value) && value==99);
 assert(!G26_CommandRxPop(&value));puts("PASS UART: bounded decimal parsing, invalid/overlong input, FIFO ownership, one-shot consumption and queue overflow");return 0;
}
