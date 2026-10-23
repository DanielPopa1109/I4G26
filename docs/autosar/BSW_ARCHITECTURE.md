# G26 BSW architecture

Implemented lab configuration, 2026-10-19. This is an AUTOSAR Classic-style implementation, not a certified AUTOSAR platform. RTE, DET and MemMap are deliberately absent.

## Execution and ownership

The existing application project remains `I4_BCC_WORKSPACE/I4_BCC`. EcuM initializes the local BSW through `Core/Src/g26_bsw.c`. ReadAll restores persistent diagnostics before application monitors run. The existing QM BSW task calls `G26_BswMainFunction` once in its 5 ms task chain. Body-control MainFunctions, UART, timers, capture interrupts and PWM remain in their existing projects and execution contexts.

The BSW scheduling order is Can, CanTp, ComM, CanNm, CanSM, Com, Dcm, NvM, Dem and the coalesced DEM persistence worker. The QM BSW task owns SPI/CAN, COM buffers, transport and DCM. Event reporting is protected by the project-local SchM_Dem critical section. The optional application command adapter reads its signal set under one interrupt mask, preserving the previous mask.

The UART ISR parses bounded decimal commands into a single-producer/single-consumer FIFO. The BTC task consumes each command once. Existing command numbers and application state machines remain. Hardware CRC access is protected against task preemption.

## Communication path

`MCP2515 -> Can -> CanIf -> CanTp -> PduR -> Dcm`

Responses follow the reverse path. CanIf routes NM3 to CanNm and application I-PDUs through PduR to Com. The actual F93 CBM transport, PduR, COM and state-manager sources were the porting baseline. G26 has independent source copies and configurations; CBM/DMU application adapters and identities were removed.

Can owns controller state, physical TX completion, timeout, error-passive observation and bus-off recovery. MCP2515 TX buffer admission is separate from TX0IF completion. Software filtering validates ID/DLC and rejects extended/RTR aliases. One hardware TX mailbox is serialized across diagnostics and application traffic.

CanTp has bounded 256-byte RX/TX contexts, extended addressing, SF/FF/CF/FC, BS, STmin, sequence checks, bounded WAIT handling, timeouts and confirmation-driven TX. Incoming response flow control uses gateway address 0x41; requests use target address 0x55; responses use tester address 0xF0. Request CAN ID is 0x710; G26 response ID is 0x709. External gateway configuration is still required.

COM uses configured signals, I-PDUs, groups, packing/unpacking, cyclic/explicit event transmission, deadlines, validity and notifications. Stopping an RX group invalidates its cached command; re-enabling requires a fresh frame. The 0x040 bit layout is source-derived, but its proposed actuator enumeration is disabled by default. Proposed status 0x241 is independently disabled.

CanNm follows the inspected NM3 0x3FF/DLC1 active value 0xFF. G26's receive timeout is a local 1000 ms policy. Zero is accepted as a local release value; inspected ZGW source establishes active transmission and cessation, not a guaranteed transmitted zero release. Normal FullCom/SilentCom/NoCom gating does not remove the diagnostic receive/response path. CanSM coordinates recovery with Can controller state.

## Diagnostic and memory services

Dcm has request ownership, dispatch/access tables, session/S3 handling, P2/P2*, NRCs, confirmation-driven reset and asynchronous NvM jobs. App callbacks implement G26 identity and a configured local persistence routine. Programming service payloads remain in the resident FBL. No second legacy APP dispatcher remains.

Dem has eight G26 events, debouncing, UDS status, confirmation, one boot operation cycle, aging, event memory, snapshots, extended data, filtering and persistent clear rollback. Monitors remain in DiagCtrl and SystemManager. The old externally writable DTC arrays are removed.

NvM has one genuine persistent block: the versioned 512-byte DEM image. No application coding/calibration schema exists in G26 to preserve, so reference ECU coding blocks were not invented. NvM queues jobs, snapshots writes, reports status, restores defaults and retries failures. MemIf stores CRC-protected 1 KiB append records with commit-last sequencing, generation selection and inactive-bank rotation. Unchanged blocks/records avoid writes.

The STM32 backend restricts erase/program to sectors 4 and 7. Busy polling/program/erase execute in .RamFunc SRAM code and feed IWDG. Cache invalidation precedes readback. An application callback coordinates diagnostic NRC78 delivery before a long erase. Single-bank flash erase still suspends normal code and IRQ processing; timing and input-loss behavior require bench validation.

## Boot execution modes

The independent `G26_FBL` owns startup, SPI/CAN, transport, UDS, APP validation/programming and updater activation. APP/FBL share identical local layout/board headers, checked automatically. No FBL source is linked from APP.

The updater is part of the FBL load image but executes from SRAM, with its own vectors, stack, full replacement buffer, clock setup, SPI/MCP2515, transport, flash controller and reset path. It validates the complete replacement before erasing resident sectors 0-3 and programs reset vectors last. Activation remains explicitly interlocked off by default.

See [compliance](COMPLIANCE_MATRIX.md), [partition](../bootloader/FINAL_MEMORY_LAYOUT.md), [updater evidence](../bootloader/RAM_UPDATER_VERIFICATION.md) and [closure](../modernization/G26_IMPLEMENTATION_CLOSURE.md).
