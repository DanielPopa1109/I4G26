# G26 changes and external changeset boundary

The current exact unimplemented external changes are in [EXTERNAL_INTEGRATION_DEPENDENCIES.md](EXTERNAL_INTEGRATION_DEPENDENCIES.md). They include G26 address/response routing, FCD target registration, partition v2 metadata, resident/APP/SRAM update workflows and review of proposed signal semantics. No outside repository was changed.

G26 locally implements the configured BSW, persistent diagnostics and independent resident/SRAM update paths. See [implementation closure](../modernization/G26_IMPLEMENTATION_CLOSURE.md), [protocol](../bootloader/PROGRAMMING_PROTOCOL.md) and [hardware dependencies](../can/PHYSICAL_HARDWARE_DEPENDENCIES.md). Proposed commands and status remain disabled; deployed interoperability is unverified.
