# G26 implementation and compliance matrix

2026-10-19. “Implemented” means functional source for the configured lab scope. It does not mean every standardized API, ECUC variation, OEM diagnostic service or formal AUTOSAR conformance requirement is supported.

| Area | Implemented behavior | Executed evidence | Scope/remaining limits |
|---|---|---|---|
| Can / MCP2515 | Explicit oscillator timing, hardware filtering, RX/TX, completion/error checks, timeout, bus-off recovery | Cortex-M builds; register-model and stack host tests | Physical oscillator, transceiver, wiring, bus-off/ACK behavior unverified |
| CanIf | Configured ID/DLC routing, serialized TX and confirmation dispatch | Native filtering and physical-confirmation model | One Classic CAN controller/mailbox |
| CanTp | Extended address SF/FF/CF/FC, BS/STmin, sequence/timeouts, early FC, callbacks | Native segmented requests/responses and failures | 256-byte transport limit; no FD/multiple concurrent connections |
| PduR | Configured TP and IF paths; DCM/COM callbacks and error counters | Native full stack tests | No DoIP/LIN stack inside G26 |
| Com | Configured I-PDUs/signals/groups, pack/unpack, cyclic/explicit event TX, deadlines, validity, notifications | Native decoding, bounds, group freshness and TX tests | No invented invalid wire encoding; proposed actuator/status semantics default off |
| CanNm / CanSM / ComM | NM3 activity/timeout, requests, normal traffic gating, recovery | Native transition/bus-off/diagnostic policy tests | Adapted ZGW lab NM3, not a claim of standard AUTOSAR NM wire format |
| Dcm | DSL ownership/timing, DSD table/access checks, DSP services, OpStatus-style persistence, NRC78, S3/reset | Native sessions, long jobs/P2*, segmented DID/snapshot/DTC, reset-confirmation tests | One connection/client; no security algorithm or coding session is configured |
| Dem | Eight configured events/DTCs, debounce/status/cycles/confirmation/aging, memory, snapshot/extended data, filter/clear | Native lifecycle, persistence, rollback tests | Local lab DTC numbers retained; no OEM DTC or sensor-snapshot contract claimed |
| NvM | Init/ReadAll/WriteAll/read/write/status/defaults/invalidate, async queue, modified tracking/retry | Native defaults, restoration, interrupted jobs and failures | One configured DEM block; no dataset/write-protected coding block configured |
| MemIf / Crc / Fls | Versioned CRC records, commit-last, history fallback, inactive-bank GC, range checks, SRAM flash-busy code | Native simulated NOR and real Cortex-M linking | Journal redundancy is not a full AUTOSAR redundant-block implementation with repair APIs |
| Resident FBL | APP erase/download/transfer/CRC/header validation, handover, reset and updater entry | Native actual linked APP replacement; Cortex-M builds | Physical programming and handover unexecuted |
| RAM updater | Full independent replacement mechanism, image/vector/tag checks, readback, vectors-last, reset | Native actual linked FBL replacement; ELF relocations/disassembly | Power loss after erase requires physical recovery; default activation off |
| Application preservation | Existing tasks/features retained; typed UART ingress, protected CRC and COM snapshot | Complete APP builds, parser tests, source audit | Real actuator/timer/sensor behavior and stack watermarks unverified |

No required named module is represented merely by an empty source file. Unconfigured security/coding services are rejected rather than acknowledged. Formal requirements beyond this configured subset remain outside the conformance claim.

Functional reference documents include AUTOSAR R24-11 [CanIf](https://www.autosar.org/fileadmin/standards/R24-11/CP/AUTOSAR_CP_SWS_CANInterface.pdf), [Dem](https://www.autosar.org/fileadmin/standards/R24-11/CP/AUTOSAR_CP_SWS_DiagnosticEventManager.pdf) and [NvM](https://www.autosar.org/fileadmin/standards/R24-11/CP/AUTOSAR_CP_SWS_NVRAMManager.pdf). In particular, aging removes event records, and NvM modified-state tracking avoids unnecessary writes. This matrix records deliberate simplifications instead of asserting full specification compliance.

See [executed validation manifest](../modernization/VALIDATION_RESULTS.json) and the four separate dependency/issue reports linked from [closure](../modernization/G26_IMPLEMENTATION_CLOSURE.md).
