# G26 repository guidance

- Start at [`docs/knowledge/INDEX.md`](docs/knowledge/INDEX.md); read only pages relevant to the task.
- Verify behavior and configuration against `I4_BCC_WORKSPACE/I4_BCC` source before changing code.
- For migrations, inspect the relevant ZGW/FCD contracts and ELC/F93 references; CBM CAN reference is under `D:\GitHub_Repos\F93\CBM - Central Body Module`.
- Preserve G26 application behavior and verify hardware dependencies before porting reference code.
- Update affected knowledge pages when implementation changes. Do not treat proposals in docs as implemented features.
- Partition v2 and verification evidence: `docs/bootloader/FINAL_MEMORY_LAYOUT.md`, `docs/modernization/G26_IMPLEMENTATION_CLOSURE.md`.
- Keep legacy board and proposed command/status transmission disabled by default. CAN verification builds explicitly assume a 16 MHz MCP2515; this is not evidence of the fitted oscillator.
- Use `Modernization/tools/build_firmware.py` and `Modernization/tests/run_tests.py`; FBL builds must pass `Modernization/tools/check_ram_closure.py`. Never deploy the updater-verification artifact during a software-only task.
