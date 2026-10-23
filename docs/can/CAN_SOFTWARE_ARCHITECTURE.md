> Historical audit/design baseline. Implementation status, partition and provisional transport assumptions below are superseded by [2026-10-19 closure](../modernization/G26_IMPLEMENTATION_CLOSURE.md), [current architecture](../autosar/BSW_ARCHITECTURE.md) and [final partition](../bootloader/FINAL_MEMORY_LAYOUT.md). This page is retained for reference; proposals are not proof of current behavior.

# CAN software architecture proposal

Keep hardware access and protocol/application mapping separate:

`MCP2515/SPI driver -> CAN controller abstraction + RX/TX queues -> CanIf-like PDU routing -> ISO-TP/UDS transport -> diagnostic service dispatcher -> application APIs`; periodic signal traffic can use explicit message codecs and freshness/timeout checks. Place interrupt work in a bounded ISR that signals a task; do not perform long SPI transactions in ISR. Include bounded queues, TX arbitration, RX overflow counters, controller reset/recovery, bus-off state and diagnostics.

G26 FreeRTOS tasks/callback contexts need a declared owner for each queue and RTE/shared value. Adapt rather than graft an AUTOSAR stack wholesale: the current RTE is not PduR/CanIf. First deliver a hardware-independent CAN frame API and codec tests; then controller driver; then transport; then only ECU-role-approved signals/services. See [RTE analysis](../knowledge/RTE_ANALYSIS.md).
