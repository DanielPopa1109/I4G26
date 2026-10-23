> Historical audit/design baseline. Implementation status, partition and provisional transport assumptions below are superseded by [2026-10-19 closure](../modernization/G26_IMPLEMENTATION_CLOSURE.md), [current architecture](../autosar/BSW_ARCHITECTURE.md) and [final partition](../bootloader/FINAL_MEMORY_LAYOUT.md). This page is retained for reference; proposals are not proof of current behavior.

# Migration checklist

- [ ] Confirm MCU marking, board revision, schematic, power and pin routing.
- [ ] Inventory runnable callers, RTE APIs, globals, task/ISR contexts and timer cadence.
- [ ] Freeze G26 ECU network role, approved signals, frame IDs, timing and DBC source.
- [ ] Decide CAN controller/transceiver and verify SPI/INT/CS pins and electrical design.
- [ ] Define ISO-TP/UDS diagnostic subset, addressing, sessions, security and FCD target values.
- [ ] Obtain exact F411 sector map; size FBL/app/state and define recovery policy.
- [ ] Create separate FBL/app builds and assert linker regions/vector alignment.
- [ ] Add typed application interfaces; keep RTE during staged migration.
- [ ] Add application regression, transport, flash simulation and HIL test evidence.
- [ ] Validate FCD, ZGW routes and recovery on bench before any in-vehicle use.
- [ ] Update docs with implemented status, source symbols, tested hardware and unresolved facts.
