> Historical audit/design baseline. Implementation status, partition and provisional transport assumptions below are superseded by [2026-10-19 closure](../modernization/G26_IMPLEMENTATION_CLOSURE.md), [current architecture](../autosar/BSW_ARCHITECTURE.md) and [final partition](../bootloader/FINAL_MEMORY_LAYOUT.md). This page is retained for reference; proposals are not proof of current behavior.

# CAN hardware options

## G26 baseline

STM32F411RETx has no integrated CAN controller; `.ioc` configures no SPI either. Board-level fitted components and spare pins are unknown. No CAN transceiver/controller can be assumed.

## Options

1. External MCP2515 over SPI: Classic CAN controller, 11/29-bit identifiers, up to 1 Mbit/s nominal; requires an external CAN physical transceiver, oscillator, CS, SCK/MOSI/MISO, optional INT/reset, supply/logic compatibility and termination strategy. CAN FD unsupported. This is closest to CBM's source but requires hardware design and throughput/timing budget.
2. MCU/board with integrated CAN: choose only after system/board redesign; a controller still requires a compliant physical transceiver. F411 is not such an option.
3. Alternative external controller: select by required classic/FD, SPI throughput, driver maturity, supply and automotive qualification. FD requires an FD-capable controller and transceiver; validate network topology and ZGW bus.

Decision gate: inspect schematics/PCB, target bus type/bitrate, pin availability, electrical protection, power/reset behavior and timing needs. MCP2515 plus transceiver is the practical same-MCU Classic-CAN proposal, not an existing G26 feature.
