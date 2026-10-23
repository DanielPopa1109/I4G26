# Memory and build

Use [partition v2](../bootloader/FINAL_MEMORY_LAYOUT.md): FBL 64 KiB sectors 0–3; NvM A sector 4; APP header at 0x08020000 and vectors at 0x08020200, sectors 5–6; NvM B sector 7. APP/FBL reserve the top 1 KiB SRAM mailbox. The FBL has distinct updater, updater-stack and resident RAM regions. Both independent linkers and local layout headers agree.

Both Cube projects use relative links to official vendor sources under ignored .build-deps/cube. APP has all source trees in Debug/Release and explicit guarded-CAN configurations. Build with Modernization/tools/build_firmware.py; legacy and CAN Release/Debug plus updater-enabled FBL have executed full GNU Cortex-M builds. The compiler and source mappings are recorded; no CubeIDE GUI build is claimed. Do not use old generated Debug/Release maps as current evidence or the legacy RAM linker as an installation image.

See [closure](../modernization/G26_IMPLEMENTATION_CLOSURE.md) and [artifact/memory manifest](../modernization/VALIDATION_RESULTS.json). Partition migration requires coordinated FBL/APP handling; the new NvM APP alone is unsafe against the former partition. No physical migration occurred.
