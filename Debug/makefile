################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := F:/ti/ccsv6/tools/compiler/ti-cgt-msp430_15.12.1.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./main.obj" \
"./AFE4400/AFE4400.obj" \
"./HAL/HAL_FLASH.obj" \
"./HAL/HAL_PMAP.obj" \
"./HAL/HAL_PMM.obj" \
"./HAL/HAL_TLV.obj" \
"./HAL/HAL_UCS.obj" \
"./OLED/oled.obj" \
"./PLUS/BH1750FVI.obj" \
"./PLUS/HTU21D.obj" \
"./PLUS/UV6070.obj" \
"../lnk_msp430f5529.cmd" \
$(GEN_CMDS__FLAG) \
-llibmath.a \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include AFE4400/subdir_vars.mk
-include HAL/subdir_vars.mk
-include OLED/subdir_vars.mk
-include PLUS/subdir_vars.mk
-include subdir_rules.mk
-include AFE4400/subdir_rules.mk
-include HAL/subdir_rules.mk
-include OLED/subdir_rules.mk
-include PLUS/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
AFE4400beta.out \

EXE_OUTPUTS__QUOTED += \
"AFE4400beta.out" \

BIN_OUTPUTS += \
AFE4400beta.hex \

BIN_OUTPUTS__QUOTED += \
"AFE4400beta.hex" \


# All Target
all: AFE4400beta.out

# Tool invocations
AFE4400beta.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: MSP430 Linker'
	"F:/ti/ccsv6/tools/compiler/ti-cgt-msp430_15.12.1.LTS/bin/cl430" -vmspx --data_model=large --code_model=large --opt_for_speed=1 --use_hw_mpy=F5 --advice:power="all" -g --define=__MSP430F5529__ --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=minimal -z -m"AFE4400beta.map" --stack_size=320 --heap_size=320 --cinit_hold_wdt=on -i"F:/ti/ccsv6/ccs_base/msp430/include" -i"F:/ti/ccsv6/ccs_base/msp430/lib/5xx_6xx_FRxx" -i"F:/ti/ccsv6/tools/compiler/ti-cgt-msp430_15.12.1.LTS/lib" -i"F:/ti/ccsv6/tools/compiler/ti-cgt-msp430_15.12.1.LTS/include" --reread_libs --warn_sections --display_error_number --diag_wrap=off --xml_link_info="AFE4400beta_linkInfo.xml" --use_hw_mpy=F5 --rom_model -o "AFE4400beta.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

AFE4400beta.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: MSP430 Hex Utility'
	"F:/ti/ccsv6/tools/compiler/ti-cgt-msp430_15.12.1.LTS/bin/hex430" --memwidth=8 --romwidth=8 -o "AFE4400beta.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)$(BIN_OUTPUTS__QUOTED)
	-$(RM) "main.d" "AFE4400\AFE4400.d" "HAL\HAL_FLASH.d" "HAL\HAL_PMAP.d" "HAL\HAL_PMM.d" "HAL\HAL_TLV.d" "HAL\HAL_UCS.d" "OLED\oled.d" "PLUS\BH1750FVI.d" "PLUS\HTU21D.d" "PLUS\UV6070.d" 
	-$(RM) "main.obj" "AFE4400\AFE4400.obj" "HAL\HAL_FLASH.obj" "HAL\HAL_PMAP.obj" "HAL\HAL_PMM.obj" "HAL\HAL_TLV.obj" "HAL\HAL_UCS.obj" "OLED\oled.obj" "PLUS\BH1750FVI.obj" "PLUS\HTU21D.obj" "PLUS\UV6070.obj" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

