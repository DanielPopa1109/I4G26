#ifndef G26_BOARD_H
#define G26_BOARD_H
#ifndef G26_MCP2515_REWIRED_BOARD
#define G26_MCP2515_REWIRED_BOARD 0
#endif
/* No oscillator is inferred from another ECU. A CAN build must select it. */
#ifndef G26_MCP_OSC_HZ
#define G26_MCP_OSC_HZ 0U
#endif
#ifndef G26_ENABLE_PROPOSED_BODY_COMMANDS
#define G26_ENABLE_PROPOSED_BODY_COMMANDS 0
#endif
#ifndef G26_ENABLE_PROPOSED_STATUS
#define G26_ENABLE_PROPOSED_STATUS 0
#endif
#if G26_MCP2515_REWIRED_BOARD != 0 && G26_MCP2515_REWIRED_BOARD != 1
#error Invalid G26 board variant
#endif
#if G26_MCP2515_REWIRED_BOARD && G26_MCP_OSC_HZ != 8000000U && G26_MCP_OSC_HZ != 16000000U && G26_MCP_OSC_HZ != 20000000U
#error Select the fitted MCP2515 oscillator explicitly: 8, 16 or 20 MHz
#endif
#endif
