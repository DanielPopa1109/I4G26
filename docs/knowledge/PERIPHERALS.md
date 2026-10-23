# Peripherals and execution contexts

Configuration evidence is `.ioc` plus `Core/Src` and `IOServices`. Treat `I4_BCC.txt` and `BCC_PinConfig.csv` as design artifacts and reconcile them with generated runtime configuration before wiring or deployment.

- GPIO: application pin I/O is routed through `PortH` and direct HAL calls after RTE removal. `BCC_PinConfig.csv` is a logical mapping, not proof of board wiring. The guarded CAN variant has unresolved rewiring dependencies; see [hardware dependencies](../can/PHYSICAL_HARDWARE_DEPENDENCIES.md).
- ADC1: `.ioc` configures 12-bit channels 12, 13, and internal temperature sensor; circular DMA2 Stream0, 3-word buffer in `AdcH`; analog watchdog is configured. However, `I4_BCC.txt` lists many additional ADC pins/channels (2–15) and `AdcH.c` configures a subset that should be reconciled against the `.ioc`. Validate channel order, DMA mapping and threshold semantics before relying on measurements.
- TIM2/3/4: PWM outputs; TIM5: two input-capture channels. Application timers/capture callbacks run asynchronously to tasks. `TimH` reinitializes after errors and increments system fault counters.
- USART1: asynchronous 9600 baud, bounded command RX/FIFO via `UartH`/G26_CommandRx/BTC. ISR and consumer ownership are host-tested; physical timing remains unverified.
- IWDG: configured (prescaler/reload in `iwdg.c`); WatchdogManager feeds during normal execution, and independent SRAM busy helpers feed during flash operations. Check timeout and scheduling against the actual board clock.
- CRC peripheral is configured in generated code; `SystemServices/Crc` owns its protected use. NvM/update records use separate software IEEE CRC32.
- No G26 SPI, CAN, I2C, RTC, or external nonvolatile memory instance is configured in `.ioc`. `I4_BCC.txt` lists I2C1/I2C3 pin assignments (PB6/PB7 and PA8/PB4), but those instances are absent from the `.ioc` and runtime init. Cube callback registry options do not mean an IP is enabled. Resolve this config/design drift before using those pins.

Interrupt vectors and priorities are in `Core/Src/stm32f4xx_it.c` and `HAL_*_MSP` sources. Audit ISR-shared values and FreeRTOS `FromISR` usage before modifying callback behavior.

The retained legacy APP .ioc omits the guarded SPI initialization deliberately. Runtime CAN builds explicitly select SPI1 PA5/6/7 and PC4 CS, conflicting with legacy logical assignments unless rewired. Do not regenerate Cube code over the guards or enable the variant based on a successful build alone. FBL uses its own SPI configuration; SRAM updater independently initializes HSI16/SPI/MCP2515.
