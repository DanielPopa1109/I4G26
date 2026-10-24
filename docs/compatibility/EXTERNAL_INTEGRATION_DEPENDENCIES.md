# External ZGW/FCD dependencies

Inspected source remains the interoperability authority. ZGW firmware source, the active RepoCM5 FCD source and ZGW's FCD copy have been updated for the G26 diagnostic/programming route. No gateway or ECU was flashed.

## Confirmed definitions

ZGW_CAN_3.dbc defines StatusBodyData1 0x040/DLC4 from ZGW to FRBE/DMU, with little-endian unsigned signals and scale 1/offset 0: fog bit0 width3, gearbox bit3 width3, high beam bit6 width2, ignition bit8 width4, turn bit12 width4, light sensor bit16 width8, RLS bit24 width5, central lock bit29 width3. Numeric ranges are the representable widths. These definitions do not establish G26 actuator enumerations.

ZGW Com.c configures StatusBodyData1 at 24 ticks and NM3 at 20 ticks. NM3 is 0x3FF/DLC1; CanNm writes active value 0xFF. Actual wall time follows ZGW's scheduler and COM overrides; G26 does not assert a vehicle-measured period. G26's 500 ms body deadline and 1000 ms NM timeout are explicit local policies.

## Source integration and remaining gates

1. Configured in ZGW source: M_CAN Classic acceptance filter for `0x709`, dedicated G26 response RX `0x709`, request TX `0x710`, CanTp/PduR/DCM PDU 12, route `0x55`, tester response EA `0xF0`, and gateway response flow-control EA `0x41`. G26 requests use real FC and require real responses. A TASKING firmware build, bus trace and connected segmentation test remain open.
2. Configured in FCD source: discoverable G26/I4G26 route `0x55`, contiguous APP/FBL image preflight, vector/partition checks, APP `G26A2` and resident FBL `G26F2` identity probes, routed NRC78 handling, and CRC32 TransferExit. APP payload begins at `0x08020200`; resident FBL payload begins at `0x08000000`. The FBL creates the 512-byte APP header.
3. FCD APP flow enters extended session, disables normal communication and DTC setting, enters programming, confirms resident identity, then erases/downloads/transfers/exits and checks APP restart. FCD FBL flow skips ZGW-only RID `0200`, requests G26 RID `0155`, then uses SRAM erase/download/transfer/exit. G26 keeps `0155` disabled by default and rejects it with NRC22; source compatibility does not authorize physical FBL replacement.
4. Verify fitted MCP2515 wiring/oscillator, gateway build and route traffic, real APP programming with reset/readback and power-interruption behavior. Establish a resident recovery path before enabling the FBL updater interlock. Preserve the proposed body command/status guards until their signal semantics are reviewed.
5. Review actuator value enumerations before enabling G26_ENABLE_PROPOSED_BODY_COMMANDS. The existing DBC does not authorize bit values to drive G26 outputs.
6. Review allocation/layout of proposed 0x241/DLC4 before enabling G26_ENABLE_PROPOSED_STATUS; add ZGW CanIf/PduR/COM and DBC reception if adopted. No current ZGW interpretation of this proposed status is claimed.

No end-to-end FCD/ZGW/G26 session was executed. The remaining items require target firmware tooling and bench evidence; host tests alone cannot close them.
