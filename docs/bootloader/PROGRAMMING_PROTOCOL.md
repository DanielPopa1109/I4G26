# G26 programming protocol

This describes implemented local lab services. ZGW/FCD target configuration and a hardware session are still required.

All modes use Classic CAN request 0x710, target EA 0x55, response 0x709 with tester EA 0xF0. Response flow control from ZGW uses EA 0x41. APP and resident transport capacity is 256 bytes. MCP2515 transmission is physically confirmed before destructive handover/reset decisions.

## APP

Default/extended sessions: 10 01 / 10 03. APP programming entry 10 02 requires extended session, completes NvM persistence, sends the positive response and resets with a validated boot mailbox only after successful TX. TesterPresent 3E, DIDs F1A0=G26-BODY, F187=G26A2 and F186=current session are configured.

Services 19 01/02/04/06/0A, persistent 14, CommunicationControl 28, ControlDTCSetting 85 and ECUReset 11 are configured. Snapshot record 1 is six bytes under G26-local DID F1D0: event number, captured status, occurrence and three-byte allocation sequence. Extended record 1 contains occurrence, confirmation and aging. RID F001 persists diagnostic memory using INITIAL/PENDING/CANCEL-style app processing. These lab identities/records are not OEM assignments. Security/coding services are unconfigured.

## Resident APP programming

1. Enter programming session 10 02.
2. Erase: 31 01 00 01 + 4-byte big-endian APP vector address + 4-byte image length. The resident FBL must physically confirm NRC78 before dispatching this erase; a failed confirmation leaves the APP untouched.
3. RequestDownload: 34 00 44 + address + length. It must match the prepared erase.
4. TransferData: 36 BSC + 1..240 bytes, starting BSC=1, modulo 256. Identical duplicates are accepted.
5. TransferExit: 37 + big-endian IEEE CRC32. The FBL must physically confirm NRC78 before CRC validation and header commit. Length, readback and vectors must validate; metadata/magic are committed only after integrity checks.
6. ECUReset: 11 01, after physical positive-response transmission.

APP programming erases sectors 5–6 only. Invalid/interrupted images do not pass boot validation.

## Resident-FBL replacement in SRAM

Controlled resident RID 31 01 01 55 prepares/copies the linked SRAM image. Its activation switch defaults off and returns NRC22. The enabled path confirms NRC78 before handover; SRAM sends final 71 01 01 55 00 after taking over hardware.

SRAM erase/download addresses are exactly 0x08000000, size at most 65536 bytes. Erase preparation only authorizes a transfer; physical erase is deferred until the complete image and TransferExit CRC/vector/tag checks pass. TransferData permits up to 254 data bytes. Commit sends confirmed NRC78, erases resident sectors 0–3, programs vectors last, verifies readback/CRC and permits reset only after success.

Power loss after resident erase requires separately demonstrated physical recovery. No physical activation was performed.
