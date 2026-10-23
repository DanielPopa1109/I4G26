> Historical audit/design baseline. Implementation status, partition and provisional transport assumptions below are superseded by [2026-10-19 closure](../modernization/G26_IMPLEMENTATION_CLOSURE.md), [current architecture](../autosar/BSW_ARCHITECTURE.md) and [final partition](../bootloader/FINAL_MEMORY_LAYOUT.md). This page is retained for reference; proposals are not proof of current behavior.

# Dependency-aware roadmap

1. **Baseline (software):** capture current behavior, task/ISR ownership, pin map, component state and build tool versions. Verify board schematic and MCU marking (hardware prerequisite).
2. **Architecture/RTE (software):** inventory actual RTE callers and concurrency; document contracts, then migrate interface-by-interface while preserving behavior. Avoid broad deletion.
3. **Network contract + hardware (mixed):** agree ECU role, Classic/FD bus, signal/diagnostic requirements; inspect PCB and design/select controller/transceiver. This gates CAN coding.
4. **CAN foundation (mixed):** SPI/controller driver, queues, bus error handling and bench validation; then frame codecs/timeouts and approved ZGW signals.
5. **Diagnostics (software + network):** ISO-TP/PDU routing and required UDS subset with ZGW/FCD contract; bench routing and session tests.
6. **FBL design (software + hardware constraints):** confirm flash sectors/capacity/recovery; create independent FBL/linkers and app vector/update-state changes; implement flash and recovery before transport.
7. **FBL transport/FCD (mixed):** integrate CAN/ISO-TP, exact UDS programming, target profile and gateway route; exercise interruptions on bench.
8. **Release validation:** static/build checks, application regression, HIL, bus load/timing, power-loss recovery and documented rollback. Vehicle use only after safety and recovery gates.

Dependencies: network contract and board inspection precede CAN; flash map precedes FBL linker; transport contract precedes UDS/FCD; interface inventory precedes RTE changes. Preserve lamp, locking, alarm, PDC and sensor behavior at every stage.
