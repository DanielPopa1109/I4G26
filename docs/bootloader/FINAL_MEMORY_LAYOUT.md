# Final G26 partition v2

The partition is based on STM32F411RET6 sector geometry and newly linked Cortex-M4 APP/FBL images. The previous checked-in Debug map used about 64 KiB of APP flash; its tiny Release map omitted the full application and was not accepted as sizing evidence. New complete builds fit the partition with substantial margin.

| Owner | Inclusive range | Capacity | Sector |
|---|---|---:|---|
| Resident FBL, identity tag, updater load image | 0x08000000–0x0800FFFF | 64 KiB | 0–3, four 16 KiB sectors |
| NvM journal bank A | 0x08010000–0x0801FFFF | 64 KiB | 4 |
| APP image header | 0x08020000–0x080201FF | 512 B | beginning of 5 |
| APP vectors/code/load data | 0x08020200–0x0805FFFF | 261632 B | 5–6 |
| NvM journal bank B | 0x08060000–0x0807FFFF | 128 KiB | 7 |

The header shares APP erase ownership. It is not independently erased. FBL APP erase is exactly sectors 5–6; NvM erases exactly sector 4 or sector 7; the RAM updater erases exactly sectors 0–3. No persistent data is placed inside an APP code sector. No on-flash updater staging sector is needed: the replacement FBL is buffered in SRAM.

The APP vector address remains 0x08020200. G26 image magic 0x47323631 and 16-byte header semantics remain: magic, image length, IEEE CRC32, vector address. Metadata is programmed before magic. The APP maximum vector payload is now 261632 bytes; old 384 KiB APPs are rejected. Local APP/FBL layout headers are identical and statically checked.

## SRAM

APP uses 0x20000000–0x2001FBFF (127 KiB); MSP is 0x2001FC00. The top KiB is excluded from both linkers and contains the reset mailbox at 0x2001FC00.

FBL reserves 0x20000000–0x20017FFF for updater code/data/BSS (96 KiB), 0x20018000–0x20018FFF for its 4 KiB stack, and 0x20019000–0x2001FBFF for resident RAM (27 KiB). Updater MSP is 0x20019000; resident MSP is 0x2001FC00. Linker assertions enforce all boundaries.

Measured image utilization and SHA-256 values are in [VALIDATION_RESULTS.json](../modernization/VALIDATION_RESULTS.json). Full CAN Release APP occupies approximately 54 KiB; Debug APP approximately 91 KiB; the updater-enabled resident FBL is below 28 KiB even in Debug. These are linked-image measurements, not stack high-water or hardware measurements.

## Migration boundary

Partition v2 must not run against an old 128 KiB FBL. Sector 4 previously belonged to the resident FBL and sector 7 to the APP. Installing an NvM-enabled APP alone could erase old firmware. Future physical migration requires backup/recovery access and a coordinated FBL+APP replacement after the old images and protection state are inspected. No provisioning, flashing or partition migration was executed.

Geometry reference: [ST RM0383](https://www.st.com/resource/en/reference_manual/dm00119316-stm32f411xce-advanced-armbased-32bit-mcus-stmicroelectronics.pdf). Both linkers and all three flash owners use this partition.
