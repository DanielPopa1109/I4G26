# G26 architecture

Current source is the independent APP in I4_BCC_WORKSPACE/I4_BCC and resident loader in G26_FBL. The hand-written RTE has been removed. Application tasks call component interfaces directly; the existing QM scheduling path runs G26_BswMainFunction every 5 ms.

APP initialization restores DEM through NvM before monitors, initializes the communication/diagnostic stack and enables the existing tasks. Runtime paths are MCP2515 → Can → CanIf → PduR/Com or CanTp/PduR/Dcm; Dem → NvM → MemIf/Crc/Fls provides persistence. ComM/CanNm/CanSM control normal communication without removing diagnostic reception.

See [BSW architecture](../autosar/BSW_ARCHITECTURE.md), [application](APPLICATION.md), [diagnostics](DIAGNOSTICS.md) and [current implementation/validation](../modernization/G26_IMPLEMENTATION_CLOSURE.md). Hardware configuration and proposed actuator/status mappings remain guarded; source implementation does not imply board readiness.
