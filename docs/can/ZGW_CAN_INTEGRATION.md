> Historical audit/design baseline. Implementation status, partition and provisional transport assumptions below are superseded by [2026-10-19 closure](../modernization/G26_IMPLEMENTATION_CLOSURE.md), [current architecture](../autosar/BSW_ARCHITECTURE.md) and [final partition](../bootloader/FINAL_MEMORY_LAYOUT.md). This page is retained for reference; proposals are not proof of current behavior.

# ZGW CAN integration

ZGW compatibility sources inspected: `docs/compatibility/NETWORK_CONTRACTS.md`, `DIAGNOSTIC_CONTRACTS.md`, `FLASHING_CONTRACTS.md`, plus ZGW CanIf/CanTp configuration. ZGW has Classic CAN and CAN FD networks, with configured diagnostic physical identifier `0x710` and target-specific routing/transport rules. The presence of both networks does not mean G26 belongs to both. Select the G26 ECU role/network and frame contract from authoritative per-target configuration/DBC.

For Classic CAN, keep payloads at 8 bytes and use the exact ID, DLC, signal endian/scaling, cadence and alive/timeout policy from the chosen contract. For diagnostics, confirm tester/ECU addressing, ISO-TP addressing mode, flow control, block/STmin/timeouts, UDS session/security and routing. FCD profile entries and ZGW routes must agree. Do not reuse ZGW's local diagnostic ID blindly for a routed ECU.

Current G26 has none of the network, TP, PDU routing or UDS layers. Implement only required body signals and diagnostics, and flag any ZGW/FCD contract mismatch at its owning layer. The ZGW docs are configuration evidence, not measurement of a connected bus.
