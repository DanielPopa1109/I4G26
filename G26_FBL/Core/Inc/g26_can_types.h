#ifndef G26_CAN_TYPES_H
#define G26_CAN_TYPES_H
#include <stdint.h>
typedef struct { uint32_t id; uint8_t dlc; uint8_t data[8]; } G26_CanFrame;
#endif
