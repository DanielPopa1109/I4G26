> Historical audit/design baseline. Implementation status, partition and provisional transport assumptions below are superseded by [2026-10-19 closure](../modernization/G26_IMPLEMENTATION_CLOSURE.md), [current architecture](../autosar/BSW_ARCHITECTURE.md) and [final partition](../bootloader/FINAL_MEMORY_LAYOUT.md). This page is retained for reference; proposals are not proof of current behavior.

# ELC/F93 FBL references

ELC (`D:\GitHub_Repos\ELC\ELC_FBL`) is STM32F103C8T6, with `can.c`, `ram_blob.c`, `STM32F103C8TX_FLASH.ld`, CAN and UDS/update flow. Its memory map, HAL, flash geometry, vector relocation and addresses are not portable to F411. Reuse protocol/state-machine ideas only after checking its source against actual ZGW/FCD contracts; do not transplant its linker file or RAM updater binary.

F93 contains `CBM - Central Body Module\CBM_FBL`, built for STM32F411RETx and therefore a closer startup/linker/platform reference. `CBM_FBL/Core/Src/main.c` is a large monolithic implementation: it declares MCP2515/CAN SPI routines, RequestDownload/TransferData/TransferExit, and flash erase/write/read entry points; `Core/Src/spi.c`, `gpio.c`, `usart.c`, and F411 linker scripts are adjacent platform references. Audit the actual control flow before reuse; a similar MCU does not establish compatible addressing, flash layout, protocol, validity checks or recovery guarantees.

CBM application CAN driver is separately described in [CBM reference](../can/CBM_SPI_CAN_REFERENCE.md). Current G26 has neither FBL project nor CAN stack. Choose reusable pure protocol logic only after requirements and license audit; reimplement MCU-specific flash/startup safely.
