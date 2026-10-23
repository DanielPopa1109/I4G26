#ifndef G26_COMMAND_RX_H
#define G26_COMMAND_RX_H
#include <stdint.h>
void G26_CommandRxInit(void);
/* Single ISR producer; decimal ASCII commands, at most three digits, 0..255. */
void G26_CommandRxByte(uint8_t byte);
/* Single application-task consumer; one command is consumed once. */
uint8_t G26_CommandRxPop(uint8_t *command);
uint32_t G26_CommandRxDropped(void);
#endif
