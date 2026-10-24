# External ZGW/FCD dependencies

Inspected source remains the interoperability authority. Neither ZGW nor FCD nor any other reference repository was modified.

## Confirmed definitions

ZGW_CAN_3.dbc defines StatusBodyData1 0x040/DLC4 from ZGW to FRBE/DMU, with little-endian unsigned signals and scale 1/offset 0: fog bit0 width3, gearbox bit3 width3, high beam bit6 width2, ignition bit8 width4, turn bit12 width4, light sensor bit16 width8, RLS bit24 width5, central lock bit29 width3. Numeric ranges are the representable widths. These definitions do not establish G26 actuator enumerations.

ZGW Com.c configures StatusBodyData1 at 24 ticks and NM3 at 20 ticks. NM3 is 0x3FF/DLC1; CanNm writes active value 0xFF. Actual wall time follows ZGW's scheduler and COM overrides; G26 does not assert a vehicle-measured period. G26's 500 ms body deadline and 1000 ms NM timeout are explicit local policies.

## Required external changes, not applied

1. Allocate/review G26 identity 0x55 and response CAN ID 0x709. ZGW's generic extended-address range is insufficient proof of a complete reverse route. Add the response RX/CanTp/PduR/forwarding correlation alongside existing 0x701/703/705/707 routes, and verify gateway FC address 0x41 and tester address 0xF0.
2. Add the G26 target to ParallelFlashSwc node/tester-present and target programming configuration. Preserve the distinction between APP diagnostics, resident APP programming and SRAM resident-FBL replacement.
3. FCD.pyw DEFAULT_EXTENDED_DIAG_ADDRESSES currently lists ZGW/PDM1/HVDCDC/CBM/DMU/ELC/FRBE, without G26. Add a G26 target/address/bus profile and discoverable descriptor; reconcile any existing I4G26 display aliases.
4. Configure APP vectors 0x08020200, APP maximum 261632 bytes, resident partition 64 KiB, erase/download address/length checks, IEEE CRC32 TransferExit and the G26 header/tag. APP download payload excludes the 512-byte header; the FBL creates it.
5. APP-to-FBL sequence is extended session, programming request, persistence, confirmed positive response and reset mailbox. Resident RID 0x0155 is explicitly interlocked; SRAM then accepts a full validated replacement for 0x08000000. Recovery authorization is separate.
   The current RepoCM5 FCD generic FBL path also sends RID 0x0200 before 0x0155. G26 has no RID 0x0200 and keeps 0x0155 disabled; a G26-specific FCD procedure and verified recovery path are required before resident-FBL replacement can be called compatible.
6. Review actuator value enumerations before enabling G26_ENABLE_PROPOSED_BODY_COMMANDS. The existing DBC does not authorize bit values to drive G26 outputs.
7. Review allocation/layout of proposed 0x241/DLC4 before enabling G26_ENABLE_PROPOSED_STATUS; add ZGW CanIf/PduR/COM and DBC reception if adopted. No current ZGW interpretation of this proposed status is claimed.

No end-to-end FCD/ZGW/G26 session was executed. These are external integration dependencies, not reasons to leave local firmware paths unimplemented.
