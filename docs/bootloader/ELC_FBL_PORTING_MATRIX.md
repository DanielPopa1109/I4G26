# ELC/F93 to G26 FBL porting matrix

References were read only. ELC_FBL main.c, ram_blob.c and STM32F103C8TX_FLASH.ld establish the linked-load/SRAM-run split. The latest F93 CBM_FBL supplies the closer F411/MCP2515 adaptation. DMU application diagnostic architecture was inspected for consistency; its on-chip CAN hardware was not copied.

| Reference responsibility | G26 implementation | Adaptation |
|---|---|---|
| Resident load image and SRAM execution section | FBL linker .ram_blob/.ram_blob_bss | 96 KiB code/state area, 4 KiB stack, separate resident RAM |
| Copy/validate/VTOR/MSP handover | Fbl_Updater.c | F411 vectors/options and partition-v2 bounds |
| Independent updater protocol | ram_blob.c | G26 target 0x55, response 0x709, 64 KiB replacement limit |
| Peripheral independence | Fbl_RamPlatform.c | HSI16, DWT, SPI1 PA5/6/7 and CS PC4; explicit fitted-oscillator selection |
| Erase/program/readback | Fbl_RamFlash.c | F411 sector controller; no F103 page erase or bxCAN |
| Replacement identity | resident tag at flash+0x200 | MCU, target, resident size, APP/header layout |
| APP metadata/programming | g26_update.c/g26_uds.c | Existing G26 header retained; APP limited to sectors 5–6 |
| Recovery | RECOVERY_AND_VALIDATION.md | Physical SWD/ROM boot availability remains unproven |

The old resident staging API and empty updater entry are removed. All required sources reside in G26, and each firmware project is independently buildable. No reference repository is linked at build time.

Validation is detailed in [RAM_UPDATER_VERIFICATION.md](RAM_UPDATER_VERIFICATION.md); physical destructive activation was not performed.
