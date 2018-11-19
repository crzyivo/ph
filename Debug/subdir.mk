################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../8led.c \
../botones_antirebotes.c \
../button.c \
../latido.c \
../led.c \
../main.c \
../reversi8_2018.c \
../timer.c \
../timer2.c 

ASM_SRCS += \
../arm_functions.asm 

OBJS += \
./8led.o \
./arm_functions.o \
./botones_antirebotes.o \
./button.o \
./latido.o \
./led.o \
./main.o \
./reversi8_2018.o \
./timer.o \
./timer2.o 

C_DEPS += \
./8led.d \
./botones_antirebotes.d \
./button.d \
./latido.d \
./led.d \
./main.d \
./reversi8_2018.d \
./timer.d \
./timer2.d 

ASM_DEPS += \
./arm_functions.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Windows GCC C Compiler'
	arm-none-eabi-gcc -I"D:\PH\workspace\PH_01_Placa\common" -O0 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -mapcs-frame -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=arm7tdmi -g3 -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: ../%.asm
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Windows GCC Assembler'
	arm-none-eabi-gcc -x assembler-with-cpp -I"D:\PH\workspace\PH_01_Placa\common" -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=arm7tdmi -g3 -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


