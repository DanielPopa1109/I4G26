################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include SystemServices/WatchdogManager/subdir.mk
-include SystemServices/SystemManager/subdir.mk
-include SystemServices/EcuM/subdir.mk
-include SystemServices/Crc/subdir.mk
-include Middlewares/FreeRTOS/subdir.mk
-include IOServices/TimH/subdir.mk
-include IOServices/PortH/subdir.mk
-include IOServices/AdcH/subdir.mk
-include Drivers/STM32F4xx_HAL_Driver/subdir.mk
-include DiagnosticServices/Dem/subdir.mk
-include Core/ThreadSafe/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include CommunicationServices/Uart/subdir.mk
-include ApplicationLayer/SenCtrl/subdir.mk
-include ApplicationLayer/SecAlm/subdir.mk
-include ApplicationLayer/Pdc/subdir.mk
-include ApplicationLayer/IntLights/subdir.mk
-include ApplicationLayer/ExtLights/subdir.mk
-include ApplicationLayer/DiagCtrl/subdir.mk
-include ApplicationLayer/CenLoc/subdir.mk
-include ApplicationLayer/BTC/subdir.mk
-include Appl_Main/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := I4_BCC
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
I4_BCC.elf \

MAP_FILES += \
I4_BCC.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
I4_BCC.list \


# All Target
all: main-build

# Main-build Target
main-build: I4_BCC.elf secondary-outputs

# Tool invocations
I4_BCC.elf I4_BCC.map: $(OBJS) $(USER_OBJS) C:\Users\Daniel\Documents\BMWI4\I4_BCC_WORKSPACE\I4_BCC\STM32F411RETX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "I4_BCC.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\Daniel\Documents\BMWI4\I4_BCC_WORKSPACE\I4_BCC\STM32F411RETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="I4_BCC.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

I4_BCC.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "I4_BCC.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) I4_BCC.elf I4_BCC.list I4_BCC.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
