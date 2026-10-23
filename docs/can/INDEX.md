# G26 CAN

Current APP path is MCP2515 → Can/CanIf → Com/PduR/CanTp/Dcm, with CanNm/CanSM/ComM scheduling and policy. The independent FBL and SRAM updater use local MCP2515 transport sources. Wiring and fitted oscillator remain unverified; the legacy board is the default and proposed commands/status remain off.

- [Implemented architecture](../autosar/BSW_ARCHITECTURE.md)
- [Physical hardware dependencies](PHYSICAL_HARDWARE_DEPENDENCIES.md)
- [External signal and diagnostic contracts](../compatibility/EXTERNAL_INTEGRATION_DEPENDENCIES.md)
- [Build/test results](../modernization/G26_IMPLEMENTATION_CLOSURE.md)

Earlier CAN option/reference/integration guides are historical audit material; their provisional dispatcher, timing and implementation-status statements are superseded by the current pages.
