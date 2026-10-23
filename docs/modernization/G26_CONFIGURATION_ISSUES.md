# G26 configuration findings

- Legacy board is the default. Explicit CAN_16MHz_Debug/Release configurations and headless --variant can builds assume 16 MHz only for compilation/verification. Change the oscillator selection to the measured fitted value before any future activation.
- Proposed actuator values and status 0x241 have separate default-off switches. Selecting the CAN board alone does not enable them.
- Partition v2 is mandatory across APP/FBL/updater. Do not pair an old 128 KiB FBL with the new NvM-enabled APP.
- The only configured NvM block is diagnostic event memory. G26 has no implemented persistent coding/calibration schema; reference ECU coding blocks/identities were not copied.
- SecurityAccess, OEM routine sets, calibration/coding sessions and larger/multiple transport connections are unconfigured. They are not acknowledged with dummy success.
- Relative Cube source links point to ignored .build-deps/cube. Use bootstrap_build.py and configure_projects.py on a new host. No installed CubeIDE executable was found; GNU builds were executed with the downloaded official ARM compiler.
- APP .ioc preserves the original board. FBL .ioc describes its CAN reference hardware. CubeMX regeneration must preserve reviewed board guards, the custom BSW/task integration and partition linkers; configuration generation itself was not exercised.
