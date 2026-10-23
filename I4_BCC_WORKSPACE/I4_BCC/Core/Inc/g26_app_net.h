#ifndef G26_APP_NET_H
#define G26_APP_NET_H

#include <stdint.h>

/* Called in the application task, after local Bluetooth command processing. */
void G26_AppNetApplyCommand(void);
uint8_t G26_AppNetFullCommunication(void);

#endif
