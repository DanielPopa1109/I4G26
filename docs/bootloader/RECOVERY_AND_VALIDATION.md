# Recovery and physical validation

All work in this modernization is software-only. No firmware was flashed, deployed, erased or reset on a board. No CM5 was accessed, modified or rebooted.

Before a future board migration, establish independently usable SWD recovery, inspect BOOT/system-memory bootloader access and read option-byte/RDP/WRP/watchdog state. Reading source or linking firmware does not establish that these interfaces are physically available.

Partition-v2 migration must install a coherent FBL and APP. The old FBL owns sector 4 and old APP may own sector 7; enabling the new NvM layout against either old image is unsafe. Preserve backups and a recovery path before any separately authorized physical action.

APP programming uses header/CRC/vector checks and commits magic last. Interrupted APP programming stays in diagnostic boot mode because validation fails. RAM self-update buffers and verifies the complete candidate before erase, keeps executing from SRAM, writes vectors last and refuses intentional reset after failure. It cannot survive power loss once resident flash is erased.

Required bench work includes SPI/oscillator/transceiver verification, CAN ACK/error/bus-off tests, gateway routing, long NvM erase timing/input loss, watchdog margin, task/stack measurements, APP/FBL/SRAM transitions and physical recovery. Native tests and ELF checks are recorded in the [closure report](../modernization/G26_IMPLEMENTATION_CLOSURE.md).
