# Actual resident-FBL replacement

G26_FBL now contains Fbl_Updater.c, Fbl_RamPlatform.c, Fbl_RamFlash.c and ram_blob.c. [RAM_UPDATER_VERIFICATION.md](RAM_UPDATER_VERIFICATION.md) describes the independent SRAM execution, full-image validation, confirmed pending responses, sectors 0–3 erase, vectors-last programming and executed tests/ELF closure. The earlier placeholder install adapter is removed. Activation defaults off; physical replacement/recovery remain unexecuted.
