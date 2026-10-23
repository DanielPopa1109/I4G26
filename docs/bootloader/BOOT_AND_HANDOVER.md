# Boot and handover

Resident boot validates the committed APP header, payload length/CRC, SRAM MSP and Thumb reset vector inside the APP image. Invalid APP remains in diagnostics. APP-to-FBL entry uses the reserved reset mailbox only after persistence and a physically confirmed response. The reserved top SRAM KiB is excluded from APP/resident/updater allocations.

Updater transfer separately verifies its load image, SRAM vectors/entry/stack and hardware preconditions, copies and compares its image, masks interrupts, switches VTOR/MSP/processor state and enters independent SRAM code. No executing callback or interrupt vector may reference the erased resident image. See [SRAM verification](RAM_UPDATER_VERIFICATION.md) and [protocol](PROGRAMMING_PROTOCOL.md).

Host models and ARM ELF closure pass; physical handover, reset, watchdog timing and recovery are unverified. See [validation](../modernization/VALIDATION_RESULTS.json).
