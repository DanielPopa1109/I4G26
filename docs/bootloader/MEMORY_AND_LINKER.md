# Memory and linker authority

[FINAL_MEMORY_LAYOUT.md](FINAL_MEMORY_LAYOUT.md) is the current partition v2 authority. APP/FBL local layout headers and independent FLASH linker scripts implement those boundaries; current memory measurements and artifact fingerprints are in [validation](../modernization/VALIDATION_RESULTS.json). The older 128 KiB FBL/384 KiB APP proposal is superseded. No physical partition migration occurred.
