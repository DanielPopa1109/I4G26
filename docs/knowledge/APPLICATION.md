# Application behavior and integration

The existing ApplicationLayer BTC, ExtLights, IntLights, CenLoc, SecAlm, Pdc, SenCtrl and DiagCtrl state machines and FreeRTOS task structure remain. RTE wrappers are replaced by direct owner interfaces and calls. Existing UART command values are preserved; G26_CommandRx now bounds decimal input to 0–255, queues complete commands and prevents ISR/task buffer reuse. Shared hardware CRC access preserves interrupt state.

COM decodes the source-grounded StatusBodyData1 fields into a coherent snapshot. The optional body-command adapter remains disabled until the value-to-actuator contract and physical rewiring are established. Status 0x241 also remains disabled by default. The former writable DTC array is replaced by Dem interfaces; legacy Bluetooth reporting queries configured fault state.

Actual source/build/host evidence is in [closure](../modernization/G26_IMPLEMENTATION_CLOSURE.md). Lamp, locking, alarm, sensor, PWM/capture, UART timing and watchdog behavior still require board regression. Existing cross-task globals outside the adapted interfaces remain a concurrency review item; compiling them is not runtime proof.
