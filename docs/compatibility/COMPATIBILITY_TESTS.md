> Historical audit/design baseline. Implementation status, partition and provisional transport assumptions below are superseded by [2026-10-19 closure](../modernization/G26_IMPLEMENTATION_CLOSURE.md), [current architecture](../autosar/BSW_ARCHITECTURE.md) and [final partition](../bootloader/FINAL_MEMORY_LAYOUT.md). This page is retained for reference; proposals are not proof of current behavior.

# Compatibility test plan

## Software-only

- Unit test body state machines/codecs and regression of existing UART command behavior.
- Check RTE APIs have implementations/callers; verify typed value ownership and ISR/task synchronization.
- Test CAN signal byte order, scaling, boundaries, freshness and timeout behavior against approved DBC.
- Test ISO-TP segmentation, flow control, address mode, timeout and malformed frames.
- Test UDS session/security/service/NRC behavior and FCD request/response contract in simulation.
- Test FBL address bounds, erase geometry, alignment, BSC retries, hash, vectors and simulated power loss.
- Build both app and FBL; inspect map/sections for overlap, memory use, vector and RAM updater placement.

## Hardware-dependent

Verify SPI/controller identity and oscillator, CAN bitrate/filter/TX/RX, INT behavior, transceiver levels/termination, bus-off and recovery, ZGW routing, FCD diagnostics/programming, watchdog, reset handover, power interruption and body-feature regression. Capture trace and exact firmware hashes. Test plan only; no tests/builds were run for this documentation task.
