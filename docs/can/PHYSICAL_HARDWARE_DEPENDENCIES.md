# Physical G26 dependencies and pin ownership

Hardware evidence is absent. All wiring below describes the explicit software board variant, not verified PCB routing.

| Pin/resource | Legacy/source ownership | CAN variant |
|---|---|---|
| PA5 | Legacy GPIO output; PortH low-beam logical macro; CSV LB_R | SPI1 SCK AF5; disconnect any legacy output load |
| PA6 | Legacy GPIO output; PortH door-LED logical macro; CSV RPL_L | SPI1 MISO AF5 |
| PA7 | Legacy GPIO output; PortH brake logical macro; CSV RPL_R | SPI1 MOSI AF5 |
| PC4 | Legacy GPIO output; CSV LTS_R | Active-low MCP2515 CS, initial high, pull-up |
| PC10/11/12 | Not acquired in the default legacy variant | Proposed replacement logical output pins for PA5/6/7 |
| PD2 | Existing GPIO output, initial low | Spare candidate for a displaced PCB output; no fabricated actuator owner |
| SPI1 | Unused by the original APP | Polled CPU-owned transfers, no DMA and no MCP2515 INT IRQ |
| DMA2 Stream0 | ADC1 | Preserved; SPI has no DMA allocation |
| TIM2/3/4/5, USART1, ADC1 | Existing PWM/capture, Bluetooth UART and sensors | Preserved by APP |

The CSV labels and runtime logical macros disagree. The PWM outputs are separately configured by timer MSP code; moving an unused logical macro does not prove relocation of a physical lamp output. Trace the PCB nets and active output drivers before adopting PC10/11/12/PD2. No fourth actuator was silently invented for PC4.

Confirm MCU/order code, 8 MHz MCU HSE, MCP2515 oscillator, transceiver voltage, CS/MISO electrical behavior, termination and 500 kbit/s bus operation. CNF1/2/3 are 00/90/82 for 8 MHz, 01/90/82 for 16 MHz, and 00/BE/03 for 20 MHz. These values are mathematically checked in the register-model test; they are not oscillator measurements.

Confirm 2.7–3.6 V for the configured NvM x32 programming path, flash protection/option bytes, supply stability, SWD access, BOOT access and ROM-loader interfaces. No physical pin changes, option changes, flashing, erase, reset or deployment occurred.

Timing reference: [Microchip MCP2515 data sheet](https://ww1.microchip.com/downloads/aemDocuments/documents/APID/ProductDocuments/DataSheets/MCP2515-Family-Data-Sheet-DS20001801K.pdf).
