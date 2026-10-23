# G26 bootloader

Current implementation: independent G26_FBL resident project plus a real SRAM-resident replacement mechanism, contained entirely in G26.

- [Final partition](FINAL_MEMORY_LAYOUT.md)
- [Programming protocol](PROGRAMMING_PROTOCOL.md)
- [SRAM updater verification](RAM_UPDATER_VERIFICATION.md)
- [ELC/F93 adaptation matrix](ELC_FBL_PORTING_MATRIX.md)
- [Recovery and validation](RECOVERY_AND_VALIDATION.md)
- [Overall implementation and evidence](../modernization/G26_IMPLEMENTATION_CLOSURE.md)

Legacy comparison/design pages are historical; use these current pages and source for implementation. No physical update, reset or recovery test occurred. Updater activation defaults off.
