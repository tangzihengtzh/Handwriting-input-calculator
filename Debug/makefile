################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/SYSTEM/usart/subdir.mk
-include Drivers/SYSTEM/sys/subdir.mk
-include Drivers/SYSTEM/delay/subdir.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/Legacy/subdir.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Drivers/BSP/TOUCH/subdir.mk
-include Drivers/BSP/LED/subdir.mk
-include Drivers/BSP/LCD/subdir.mk
-include Drivers/BSP/KEY_LED/subdir.mk
-include Drivers/BSP/KEY/subdir.mk
-include Drivers/BSP/IIC/subdir.mk
-include Drivers/BSP/24CXX/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
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


BUILD_ARTIFACT_NAME := Demo8_1TFTLCD_V3
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
Demo8_1TFTLCD_V3.elf \

MAP_FILES += \
Demo8_1TFTLCD_V3.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
Demo8_1TFTLCD_V3.list \

OBJCOPY_HEX += \
Demo8_1TFTLCD_V3.hex \


# All Target
all: main-build

# Main-build Target
main-build: Demo8_1TFTLCD_V3.elf secondary-outputs

# Tool invocations
Demo8_1TFTLCD_V3.elf Demo8_1TFTLCD_V3.map: $(OBJS) $(USER_OBJS) E:\STM32_PRJ\KW407\Take_you_read_STM32Cube_efficient_development_tutorial-master\Demo8_1TFTLCD_V3\Demo8_1TFTLCD_V3\STM32F407ZGTX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "Demo8_1TFTLCD_V3.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"E:\STM32_PRJ\KW407\Take_you_read_STM32Cube_efficient_development_tutorial-master\Demo8_1TFTLCD_V3\Demo8_1TFTLCD_V3\STM32F407ZGTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="Demo8_1TFTLCD_V3.map" -Wl,--gc-sections -static -u _printf_float --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

Demo8_1TFTLCD_V3.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "Demo8_1TFTLCD_V3.list"
	@echo 'Finished building: $@'
	@echo ' '

Demo8_1TFTLCD_V3.hex: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O ihex $(EXECUTABLES) "Demo8_1TFTLCD_V3.hex"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) Demo8_1TFTLCD_V3.elf Demo8_1TFTLCD_V3.hex Demo8_1TFTLCD_V3.list Demo8_1TFTLCD_V3.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_HEX)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
