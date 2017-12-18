################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
SD/sd.obj: ../SD/sd.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"F:/ti/ccsv6/tools/compiler/ti-cgt-msp430_15.12.1.LTS/bin/cl430" -vmspx --code_model=small --data_model=small --opt_for_speed=1 --use_hw_mpy=F5 --preinclude="C:/Users/PP/Desktop/AFE4400beta/AFE4400beta/UART/uart0.h" --include_path="C:/Users/PP/Desktop/AFE4400beta/AFE4400beta/AFE4400" --include_path="C:/Users/PP/Desktop/AFE4400beta/AFE4400beta/SD" --include_path="C:/Users/PP/Desktop/AFE4400beta/AFE4400beta/UART" --include_path="C:/Users/PP/Desktop/AFE4400beta/AFE4400beta/HAL" --include_path="F:/ti/ccsv6/ccs_base/msp430/include" --include_path="C:/Users/PP/Desktop/AFE4400beta/AFE4400beta/DMA" --include_path="F:/ti/ccsv6/tools/compiler/ti-cgt-msp430_15.12.1.LTS/include" --advice:power="all" -g --define=__MSP430F5529__ --diag_warning=225 --display_error_number --diag_wrap=off --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="SD/sd.d" --obj_directory="SD" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


