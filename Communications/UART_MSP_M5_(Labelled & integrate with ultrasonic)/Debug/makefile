################################################################################
# Automatically-generated file. Do not edit!
################################################################################

CG_TOOL_ROOT := /Applications/CodeComposer/ccs1030/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./gpio_input_interrupt.obj" \
"./startup_msp432p401r_ccs.obj" \
"./system_msp432p401r.obj" \
"../gpio_input_interrupt_ccs.cmd" \
"/Users/dwdr/Downloads/msp432_driverlib_3_21_00_05/driverlib/MSP432P4xx/ccs/msp432p4xx_driverlib.lib" \
$(GEN_CMDS__FLAG) \
-llibc.a \

-include ../makefile.init

RM := rm -rf
RMDIR := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
gpio_input_interrupt.out \

EXE_OUTPUTS__QUOTED += \
"gpio_input_interrupt.out" \

BIN_OUTPUTS += \
gpio_input_interrupt.bin \

BIN_OUTPUTS__QUOTED += \
"gpio_input_interrupt.bin" \


# All Target
all: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "gpio_input_interrupt.out"

# Tool invocations
gpio_input_interrupt.out: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: Arm Linker'
	"/Applications/CodeComposer/ccs1030/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --advice:power=all --define=__MSP432P401R__ --define=TARGET_IS_MSP432P4XX --define=ccs -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number -z -m"gpio_input_interrupt.map" --stack_size=512 -i"/Applications/CodeComposer/ccs1030/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/lib" -i"/Applications/CodeComposer/ccs1030/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/include" --reread_libs --diag_wrap=off --display_error_number --warn_sections --xml_link_info="gpio_input_interrupt_linkInfo.xml" --rom_model -o "gpio_input_interrupt.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

gpio_input_interrupt.bin: $(EXE_OUTPUTS)
	@echo 'Building secondary target: "$@"'
	@echo 'Invoking: Arm Hex Utility'
	"/Applications/CodeComposer/ccs1030/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/bin/armhex" --diag_wrap=off -o "gpio_input_interrupt.bin" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building secondary target: "$@"'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "gpio_input_interrupt.obj" "startup_msp432p401r_ccs.obj" "system_msp432p401r.obj" 
	-$(RM) "gpio_input_interrupt.d" "startup_msp432p401r_ccs.d" "system_msp432p401r.d" 
	-@echo 'Finished clean'
	-@echo ' '

post-build:
	-/Applications/CodeComposer/ccs1030/ccs/utils/tiobj2bin/tiobj2bin gpio_input_interrupt.out gpio_input_interrupt.bin /Applications/CodeComposer/ccs1030/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/bin/armofd /Applications/CodeComposer/ccs1030/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/bin/armhex /Applications/CodeComposer/ccs1030/ccs/utils/tiobj2bin/mkhex4bin
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

