#ifndef G26_LAYOUT_H
#define G26_LAYOUT_H
#include <stdint.h>
/* Partition v2: every independently erased owner occupies whole sectors. */
#define G26_LAYOUT_VERSION 2U
#define FBL_RESIDENT_SIZE 0x10000U
#define FBL_APP_START 0x08020200U
#define FBL_METADATA 0x08020000U
#define FBL_BOOT_MAILBOX 0x2001FC00U
#define G26_APP_END 0x08060000U
#define G26_NVM_BANK0 0x08010000U
#define G26_NVM_SIZE0 0x10000U
#define G26_NVM_BANK1 0x08060000U
#define G26_NVM_SIZE1 0x20000U
#define G26_STACK_TOP 0x2001FC00U
#define G26_BOOT_MAGIC 0x46424C31U
#endif
