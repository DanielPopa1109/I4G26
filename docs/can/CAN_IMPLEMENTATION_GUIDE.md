> Historical audit/design baseline. Implementation status, partition and provisional transport assumptions below are superseded by [2026-10-19 closure](../modernization/G26_IMPLEMENTATION_CLOSURE.md), [current architecture](../autosar/BSW_ARCHITECTURE.md) and [final partition](../bootloader/FINAL_MEMORY_LAYOUT.md). This page is retained for reference; proposals are not proof of current behavior.

# CAN implementation guide

1. Confirm board schematic, target bus (Classic/FD), IDs/signals, baud rate, physical-layer requirements and reserved pins. No source code can resolve missing hardware facts.
2. Choose and procure controller/transceiver; wire SPI, CS, INT/reset, oscillator and protected CANH/CANL interface. Validate voltage levels and termination before ECU connection.
3. Enable SPI in `.ioc`; add low-level driver with bounded transactions and reset/config verification. Port CBM only after a focused API/error-path review.
4. Add frame types, queue ownership, CanIf-like routing, codecs, timeout/freshness counters, bus-off recovery and diagnostics. Keep ISR bounded.
5. Add ISO-TP and approved UDS services with explicit ZGW/FCD address/timeout contracts. Separate diagnostic PDUs from application signals.
6. Integrate application commands/state through typed interfaces; do not export component globals directly.
7. Validate unit codecs, mocked controller, bench bitrate/filters/error cases, bus capture, ZGW routing, FCD diagnostics and load/latency. Verify physical ECU only after bench gates.

No CAN code is implemented by this knowledge bank.
