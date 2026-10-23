# G26 implementation findings

Implemented corrections include the real BSW port, replacement of the legacy diagnostic dispatcher and writable DTC array, persistent DEM/NvM, non-overlapping flash ownership, physical CAN TX confirmations, oscillator-specific timing, gateway FC addressing, actual RAM replacement, UART buffer-overflow repair, protected hardware CRC access and COM freshness on group restart.

Remaining engineering validation:
- Single-bank NvM sector erase suspends normal application/IRQ execution. Busy code runs in SRAM and feeds IWDG, and active diagnostic erase waits for confirmed NRC78. Actual erase duration, lost UART/CAN/input samples, scheduler recovery and watchdog margin need bench measurements.
- Legacy timers, DMA snapshots, PWM/capture timing, UART DTC-report busy waits and application stack high-water behavior have source/build evidence but no runtime regression on a board.
- Linker RWX segment warnings arise from executable SRAM code/data. This Cortex-M4 build has no MPU memory-protection policy; no security isolation is claimed.
- The bounded lab BSW is not a complete standardized API/configuration platform. See the compliance matrix for unsupported variants.
- DEM changes are coalesced before journal writes; a power cut can lose changes not yet committed. CRC/commit-last protects the last committed record, not every newly observed event.

No empty success path substitutes for an implemented required execution mode. Integration readiness and physical verification remain open; these limits must not be reported as deployment approval.
