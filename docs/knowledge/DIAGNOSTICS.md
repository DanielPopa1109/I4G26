# Diagnostics and persistence

APP diagnostics use the configured BSW Dcm through CanTp/PduR. The earlier g26_app_dcm dispatcher is removed. Services include sessions, reset, TesterPresent, DID reads, DTC read/clear, communication control, DTC setting and asynchronous persistence. Reset/programming handover waits for confirmed positive TX. APP programming entry flushes NvM before its mailbox reset to the independent resident FBL. APP DID F187 returns G26A2; resident FBL DID F187 returns G26F2 for explicit FCD handover checks. Unsupported security/coding requests are rejected.

Dem manages eight retained G26 lab event/DTC identifiers with debounce, status, confirmation/aging, snapshots, extended data, filtering and persistent clear with rollback on storage failure. NvM restores its versioned 512-byte DEM block before monitors. MemIf journals CRC records in sectors 4 and 7; APP erase cannot touch these banks. Coalesced pending changes may be lost before commit on power failure.

See [BSW architecture](../autosar/BSW_ARCHITECTURE.md), [protocol](../bootloader/PROGRAMMING_PROTOCOL.md), [compliance limits](../autosar/COMPLIANCE_MATRIX.md) and [executed evidence](../modernization/VALIDATION_RESULTS.json). Local lab DTCs/snapshots are not an OEM diagnostic contract.
