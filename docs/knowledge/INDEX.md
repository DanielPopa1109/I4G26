# G26 knowledge bank

Source baseline: `I4_BCC_WORKSPACE/I4_BCC` (STM32CubeIDE project, STM32F411RETx). This bank distinguishes checked-in implementation from modernization proposals. Hardware schematics and board-level wiring are not present, so pin routing and fitted parts remain unverified.

## Pages

- [Architecture](ARCHITECTURE.md) · [Platform](STM32_PLATFORM.md) · [Application](APPLICATION.md)
- [RTE analysis](RTE_ANALYSIS.md) · [Peripherals](PERIPHERALS.md) · [Diagnostics](DIAGNOSTICS.md)
- [Memory and build](MEMORY_AND_BUILD.md) · [Tools and tests](TOOLS_AND_TESTS.md)
- [CAN index](../can/INDEX.md) · [FBL index](../bootloader/INDEX.md) · [Compatibility index](../compatibility/INDEX.md)

Primary evidence: `I4_BCC.ioc`, `Core/Src`, `BSW`, `DiagnosticServices/Dem`, `ApplicationLayer`, `Appl_Main/Appl_Main.c`, independent `G26_FBL`, `.cproject`, linker scripts and `BCC_PinConfig.csv`. RTE sources are removed. Read task-relevant source; do not infer board wiring from Nucleo settings alone.

Current authority: [2026-10-19 implementation closure](../modernization/G26_IMPLEMENTATION_CLOSURE.md), [BSW architecture](../autosar/BSW_ARCHITECTURE.md), [scope matrix](../autosar/COMPLIANCE_MATRIX.md) and [validation manifest](../modernization/VALIDATION_RESULTS.json). Source implementation, build/host evidence and physical integration are separate statuses.
