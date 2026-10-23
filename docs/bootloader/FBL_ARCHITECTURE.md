# Resident FBL architecture

G26_FBL is an independent STM32F411 project with its own startup, linker, HAL configuration and local communication sources. g26_update/g26_uds/g26_tp own APP programming and image validation; the raw STM32 adapter uses startup-copied SRAM busy helpers. The resident transmits responses to physical completion before reset or SRAM transfer.

APP vectors are 0x08020200, and APP programming owns sectors 5–6 only. Header magic commits last after payload CRC/vector/readback validation. Resident RID 0x0155 has a default-off interlock before a checked SRAM copy and handover. The independent SRAM updater owns sectors 0–3 replacement.

See [protocol](PROGRAMMING_PROTOCOL.md), [layout](FINAL_MEMORY_LAYOUT.md), [SRAM proof](RAM_UPDATER_VERIFICATION.md) and [recovery limits](RECOVERY_AND_VALIDATION.md). Formal secure boot/signature verification is not configured by the supplied lab contract. Nothing was physically flashed or reset.
