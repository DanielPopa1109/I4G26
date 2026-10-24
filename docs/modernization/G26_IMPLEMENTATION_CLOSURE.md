# G26 implementation closure — 2026-10-19

The [2026-10-24 follow-up audit](G26_APP_FBL_BEHAVIOR_AUDIT.md) supersedes this page for the current module assessment and records subsequent Fee, FBL and EcuM corrections. The build numbers below are the 2026-10-19 evidence, not measurements of later source.

The configured G26 lab architecture is source implemented, Cortex-M build verified and host tested. Integration readiness and hardware verification remain open. This is an AUTOSAR Classic-style implementation with the explicit limits in the [compliance matrix](../autosar/COMPLIANCE_MATRIX.md), not a formal AUTOSAR conformance claim.

## Delivered implementation

| Stage | Result and connected behavior |
|---|---|
| M1 references | Audited actual G26 APP, current F93 CBM/DMU, ELC updater, ZGW and FCD contracts. References were read only; G26 contains independent local sources. |
| M2 communication foundation | Can/MCP2515 physical TX confirmation/error/recovery, configured CanIf routing, extended-address CanTp SF/FF/CF/FC with timeouts/flow control, PduR TP/IF paths. |
| M3 normal communication | Com bit packing, groups, freshness, deadlines and cyclic/event TX; CanNm NM3 activity; CanSM/ComM normal-traffic policy with diagnostics retained. |
| M4 diagnostics | Configured Dcm sessions/timing/dispatch/services/asynchronous persistence/reset confirmation; Dem eight events, debounce, confirmation, aging, snapshots, extended data, filtering and persistent-clear rollback. |
| M5 persistence | NvM async jobs/defaults/status/dirty tracking/retries/ReadAll/WriteAll; CRC-checked MemIf append journal, commit-last and previous-record recovery; range-limited Fls with SRAM busy helpers. |
| M6 application | Existing body components and FreeRTOS task calls retained without RTE. COM inputs exposed through protected snapshots; proposed actuator/status behavior stays disabled. UART bounded parser/FIFO fixes ISR/task buffer ownership; shared hardware CRC is protected. |
| M7 resident FBL | Independent STM32 project, APP download/erase/transfer/CRC/vector checks, readback and header commit-last, valid-image handover and reset mailbox. |
| M8 SRAM updater | Actual independent CAN/clock/flash replacement protocol in SRAM, full candidate validation, sectors 0–3 replacement, body then vectors, readback/CRC and confirmed reset. Activation stays disabled by default. |
| M9 external contracts | Exact ZGW/FCD changes and unproven actuator/status definitions documented; no outside repository modified. |
| M10 evidence | Ten complete GNU Cortex-M APP/FBL images built; four assertion-enabled native suites and static/ELF gates passed. |

The real initialization and 5 ms scheduling path is EcuM → G26_BswInit/G26_BswMainFunction. DEM restoration completes before monitors run. DCM owns configured diagnostics through PduR/CanTp; the former application diagnostic dispatcher and writable legacy DTC mirror are removed. No RTE, DET, MemMap or placeholder updater remains in active source selections.

## Build and executed tests

Official Arm GNU 13.3.Rel1, STM32CubeF4 v1.27.1 and isolated Zig 0.13.0 were provisioned under ignored .build-deps/. Builds use actual independent Cube project source entries and linked vendor files, not the old generated Debug makefiles. No STM32CubeIDE GUI build was executed. All 92 APP and 36 FBL source units compile/link in their respective builds. Legacy and explicit 16 MHz MCP2515 CAN configurations pass Release and Debug; updater-enabled FBL also passes both. The 16 MHz build is a test configuration, not a fitted-oscillator observation.

| Linked variant | Flash bytes / capacity | Allocated RAM bytes / capacity |
|---|---:|---:|
| Legacy APP Release | 51393 / 261632 | 37448 / 130048 |
| CAN APP Release | 53341 / 261632 | 37576 / 130048 |
| CAN APP Debug | 90164 / 261632 | 37640 / 130048 |
| CAN FBL Release | 17104 / 65536 | 3088 / 27648 resident + 70528 / 98304 updater |
| Updater-enabled FBL Release | 17616 / 65536 | 3088 / 27648 resident + 70528 / 98304 updater |
| Updater-enabled FBL Debug | 26332 / 65536 | 3088 / 27648 resident + 70540 / 98304 updater |

The updater has a separate reserved 4 KiB stack; linker allocation does not measure stack high-water use. All ten builds, hashes and exact closure results are recorded in [VALIDATION_RESULTS.json](VALIDATION_RESULTS.json). Legacy/vendor compiler warnings and RWX SRAM segment warnings remain; the builds are not warning-free.

Executed commands:

```powershell
python Modernization/tools/build_firmware.py --variant legacy
python Modernization/tools/build_firmware.py --variant can
python Modernization/tools/build_firmware.py --variant updater --project fbl
python Modernization/tools/build_firmware.py --variant legacy --debug
python Modernization/tools/build_firmware.py --variant can --debug
python Modernization/tools/build_firmware.py --variant updater --project fbl --debug
python Modernization/tests/run_tests.py
python Modernization/tests/test_ram_checker.py
python Modernization/tests/check_source_integration.py
python Modernization/tests/check_app_integration.py
python Modernization/tools/record_validation.py
```

Every firmware build automatically checks SRAM flash-busy helper placement/branches. FBL builds additionally check updater vectors, load/runtime bounds, retained relocations and disassembly closure. The negative ELF test injects an erased-Flash dependency and confirms rejection.

Native tests execute real portable cores with assertions enabled (-UNDEBUG):

- Resident FBL: the actual linked APP binary passes the simulated UDS erase/download/transfer/duplicate/CRC/header/validation path; invalid input and transport/controller failures are rejected.
- APP BSW: transport/filter/confirmation/bus-off, COM/NM/communication control, DCM sessions/S3/P2/P2*/NRCs/DIDs/DTCs/reset confirmation, DEM lifecycle/aging/records, NvM restore/CRC/interruption/rotation/deduplication/errors, persistent-clear rollback and confirmed NRC78 before long erase.
- SRAM updater: maximum-size candidate and actual linked FBL replacement, segmented transport, duplicate/conflicting/missing blocks, vector/identity/CRC/range checks, erase/program/readback/interruption/TX failures, vectors-last and reset refusal on failure.
- UART: bounded decimal command parsing, malformed input, one-shot FIFO consumption and overflow handling.

Simulated NOR, controller registers and reset hooks do not constitute physical flash/CAN/reset proof.

## Partition and readiness limits

[Partition v2](../bootloader/FINAL_MEMORY_LAYOUT.md) reserves FBL sectors 0–3, NvM sector 4, APP sectors 5–6 (vectors 0x08020200) and NvM sector 7. Never install the new NvM APP alone against the old 128 KiB FBL: sector ownership changed. No partition migration was executed.

All named required modules have functional execution paths for the configured scope. Partial standardized coverage includes one controller/diagnostic client, bounded transport, local NM3 adaptation, one persistent DEM block and journal redundancy without the complete AUTOSAR redundant-block repair API. Security/coding services have no supplied G26 contract and are rejected. These limits are explicit in the compliance matrix.

Remaining work depends on external configuration and physical evidence:

- [Physical hardware dependencies](../can/PHYSICAL_HARDWARE_DEPENDENCIES.md): pin conflicts/rewiring, oscillator/transceiver, supply, watchdog/erase timing, recovery access and actuator regression.
- [G26 configuration issues](G26_CONFIGURATION_ISSUES.md): defaults/interlocks, proposed signal semantics, selected policies and board/profile choices.
- [External ZGW/FCD dependencies](../compatibility/EXTERNAL_INTEGRATION_DEPENDENCIES.md): route/address/response, target metadata, programming partition/protocol and adopted DBC changes.
- [Local implementation validation](G26_LOCAL_IMPLEMENTATION_ISSUES.md): single-bank erase stalls, legacy concurrency/timing and stack/runtime measurements.

Persistence commits are atomic at the journal-record level. Events waiting for the coalesced write can be lost on power failure; this is not zero-loss logging. Resident-FBL power loss after erase requires physical recovery; the RAM updater cannot survive loss of SRAM. Activation remains interlocked.

## Files and operations

Modified areas: APP BSW/diagnostics/application adapters/UART/CRC/startup/project/linker/board files; independent G26_FBL project with local resident/updater/MCAL source; Modernization build/test tools; .gitignore, AGENTS.md and affected documentation. The pre-existing dirty tree and earlier RTE/generated-Debug changes were preserved; unrelated G26.zip was left untouched. Build artifacts and dependencies are ignored.

Software deployed: none. No hardware programming/erase/reset, option-byte modification, commit, push or external repository edit occurred. The CM5 was neither accessed, modified nor rebooted.
