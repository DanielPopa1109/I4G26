# RAM updater implementation and verification

2026-10-19. The previous empty updater entry/install adapter has been removed. The implementation now comprises:
- `Core/Src/ram_blob.c`: independent buffered replacement protocol and state.
- `Fbl_RamPlatform.c`: SRAM vectors, HSI16 clock, DWT time, SPI1/MCP2515 and reset.
- `Fbl_RamFlash.c`: bounded STM32F411 erase/program/readback/cache handling.
- `Fbl_Updater.c`: resident-side precondition checks, copy verification and VTOR/MSP transfer.
- `Fbl_Ram.h`, `g26_layout.h` and the FBL linker script: execution/storage boundaries and identity.

## Activation and replacement

Resident programming session RID 0x0155 prepares the SRAM image. A controlled build must explicitly set FBL_UPDATER_ACTIVATION_ALLOWED=1 and the CAN board/oscillator configuration. Defaults return NRC22. The enabled code was compiled and inspected; no activation was executed on hardware.

Prepare checks thread context, software-watchdog option policy, resident-sector protection, WWDG state, idle flash, load/VMA bounds, stack, Thumb entry and all 112 SRAM vector entries. The resident copies and compares the image, masks/disables pending IRQs and timers, changes VTOR/MSP and processor masks, and branches without accessing its former stack.

SRAM initializes its own HSI16 clock, DWT timer and SPI/MCP2515. It accepts only the G26 extended diagnostic route. The full replacement image is buffered in SRAM. It verifies length, CRC, stack, reset/exception vectors and the resident tag (target 0x55, MCU 0x411, resident size, APP vector/header addresses). Only then does a confirmed NRC78 precede erase of sectors 0–3. It programs the image body first, vectors last, checks every readback and final CRC, and allows ECU reset only after successful commit. Exact duplicate transfer blocks are accepted; conflicting duplicates, missing blocks and invalid targets fail.

## Executed evidence

`Modernization/tests/run_tests.py` executes the real updater protocol against a simulated NOR backend. Tests cover a maximum-size candidate and the actual linked G26_FBL.bin, SF/FF/CF/FC reception, overflow/sequence/timeouts, duplicate blocks, identity/vector/range/CRC failures, confirmed pending response, erase/program/readback failures and reset refusal after interruption.

`Modernization/tools/check_ram_closure.py` reads ARM ELF sections/relocations and disassembles .ram_blob. It validates load bounds, vectors, BSS/stack separation, rejects any updater relocation outside its SRAM, direct branches outside the blob and indirect calls. Release and Debug updater-enabled images pass. The first inspection caught a compiler-generated resident memset dependency; volatile initialization removed it. Source attributes prevent loop-to-library and jump-table transformations in updater functions.

The verified Release updater has 4440 B code/data and 66088 B BSS; its end is 0x20011380, below the 0x20018000 stack bottom. Debug ends at 0x2001138C. Exact measurements, checked relocation counts and artifact hashes are in [validation](../modernization/VALIDATION_RESULTS.json). Source maps, disassembly, stack-usage files and logs are under ignored build/ directories. A negative ELF test injects an erased-Flash reference and verifies rejection. Independent checks also validate startup-copied SRAM helpers for APP NvM and resident APP programming.

CubeIDE FBL configurations retain relocation information and run the checker as a post-build step. The actual validation performed here used GNU Cortex-M4 builds from the project source mapping; no CubeIDE GUI session was run.

## Recovery limits

Power loss after resident erase destroys the SRAM updater. No automatic power-loss recovery is claimed. SWD and the STM32 system-memory bootloader are recovery candidates, conditional on physical connector/BOOT access and RDP/WRP/watchdog options. Their availability on this board is unverified. The activation interlock remains off until recovery and physical timing are demonstrated separately.

Native simulations and ELF closure do not prove physical flash, CAN, reset, IRQ handover, electrical supply or recovery behavior.
