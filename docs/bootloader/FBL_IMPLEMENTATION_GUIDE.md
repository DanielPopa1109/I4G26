> Historical audit/design baseline. Implementation status, partition and provisional transport assumptions below are superseded by [2026-10-19 closure](../modernization/G26_IMPLEMENTATION_CLOSURE.md), [current architecture](../autosar/BSW_ARCHITECTURE.md) and [final partition](../bootloader/FINAL_MEMORY_LAYOUT.md). This page is retained for reference; proposals are not proof of current behavior.

# FBL implementation guide

1. Freeze target-specific ZGW/FCD programming contract and exact MCU/board/flash data.
2. Size image/FBL/state regions on F411 sector boundaries; define single-slot recovery policy before linkers.
3. Create independent CubeIDE FBL project from minimal F411 startup configuration; pin dependency/tool versions.
4. Implement reset policy, vector validation and handover before transport; validate with a harmless test app.
5. Add flash abstraction and power-safe update state; unit-test ranges and simulated failures.
6. Add CAN hardware/driver and ISO-TP once hardware is approved; map exact UDS services and timing.
7. Align application linker/startup/update request; preserve body functions and RTE migration separately.
8. Configure FCD target profile and ZGW route to exact contract; test one target on bench.
9. Run recovery/fault injection and regression gates in [recovery plan](RECOVERY_AND_VALIDATION.md).

No FBL project, CAN code, linker changes or application changes are created by this documentation task.
