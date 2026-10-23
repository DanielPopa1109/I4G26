> Historical audit/design baseline. Implementation status, partition and provisional transport assumptions below are superseded by [2026-10-19 closure](../modernization/G26_IMPLEMENTATION_CLOSURE.md), [current architecture](../autosar/BSW_ARCHITECTURE.md) and [final partition](../bootloader/FINAL_MEMORY_LAYOUT.md). This page is retained for reference; proposals are not proof of current behavior.

# ZGW/FCD gap analysis

G26 currently presents application behavior over UART1/BTC and local I/O. Source/config does not evidence CAN, CAN FD, ISO-TP, PDU routing, UDS DCM, programming services, network management or FCD target profile. `DiagCtrl`/`Dem` are not transport/server equivalents. Therefore routed diagnostics, coding and flashing are not currently compatible based on repository evidence.

ZGW supports multiple CAN/FD diagnostic routes and FCD defines target-specific programming contracts. Select only the network/address, signals and UDS services relevant to this body ECU. Add IDs, DLC, payload/endian/scaling, timing, timeout and startup state from a validated G26 contract; do not map guessed body signals. Define physical addressing and ISO-TP, sessions, security, DIDs/routines, transfer limits, BSC and integrity algorithm with ZGW/FCD owner.

FCD must gain a G26 target entry only after these values and ECU role are approved. Any disagreement in gateway route, DBC/CanIf configuration or target profile belongs to the contract-owning ZGW/FCD layer and should be reported rather than patched with guessed G26 behavior. Sources: ZGW `docs/compatibility/{NETWORK,DIAGNOSTIC,FLASHING}_CONTRACTS.md`, G26 `.ioc`, `UartH`, `DiagCtrl`, `Dem`.
