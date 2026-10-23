# STM32 platform

`I4_BCC.ioc` names `STM32F411R(C-E)Tx`, LQFP64; project device target and linker filenames specify STM32F411RETx. The selected part is STM32F411RETx. It is Cortex-M4, with 512 KiB flash and 128 KiB SRAM according to linker regions in `STM32F411RETX_FLASH.ld` / `_RAM.ld`; verify exact order code and package against the physical board before pin or voltage decisions.

The Cube configuration sets HSE 8 MHz, PLL M=4/N=100, SYSCLK/HCLK 100 MHz, APB1 50 MHz with timer clock 100 MHz, APB2 100 MHz. LSI is configured for RTC/IWDG-related clocks. `Core/Src/main.c`, `system_stm32f4xx.c`, `stm32f4xx_hal_msp.c` and `.ioc` are primary startup/config sources.

No bxCAN peripheral is available on STM32F411. The legacy APP `.ioc` has no SPI/CAN instance; its SPI1 source is guarded behind `G26_MCP2515_REWIRED_BOARD`. The new FBL `.ioc` configures SPI1 PA5-PA7 and MCP2515 CS PC4, but actual board wiring remains unverified. CAN FD is not provided by an MCP2515. Do not infer Nucleo board wiring or automotive electrical protection from the IDE board preset. See [hardware options](../can/CAN_HARDWARE_OPTIONS.md).
