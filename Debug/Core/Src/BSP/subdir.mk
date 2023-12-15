################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/BSP/stm32f769i_discovery_lcd.c 

OBJS += \
./Core/Src/BSP/stm32f769i_discovery_lcd.o 

C_DEPS += \
./Core/Src/BSP/stm32f769i_discovery_lcd.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/BSP/%.o Core/Src/BSP/%.su Core/Src/BSP/%.cyclo: ../Core/Src/BSP/%.c Core/Src/BSP/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F769xx -c -I../Core/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1 -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-BSP

clean-Core-2f-Src-2f-BSP:
	-$(RM) ./Core/Src/BSP/stm32f769i_discovery_lcd.cyclo ./Core/Src/BSP/stm32f769i_discovery_lcd.d ./Core/Src/BSP/stm32f769i_discovery_lcd.o ./Core/Src/BSP/stm32f769i_discovery_lcd.su

.PHONY: clean-Core-2f-Src-2f-BSP

