################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
Drivers/OLED_Software_I2C/%.o: ../Drivers/OLED_Software_I2C/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Arm Compiler - building file: "$<"'
	"D:/TI_ti/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"C:/Users/HP/workspace_ccstheia/imu660rb-fusion-oled-hardware-spi/Drivers/IMU660RB/Fusion" -I"C:/Users/HP/workspace_ccstheia/imu660rb-fusion-oled-hardware-spi/Drivers/IMU660RB" -I"C:/Users/HP/workspace_ccstheia/imu660rb-fusion-oled-hardware-spi/Drivers/LSM6DSV16X" -I"C:/Users/HP/workspace_ccstheia/imu660rb-fusion-oled-hardware-spi/Drivers/VL53L0X" -I"C:/Users/HP/workspace_ccstheia/imu660rb-fusion-oled-hardware-spi/Drivers/WIT" -I"C:/Users/HP/workspace_ccstheia/imu660rb-fusion-oled-hardware-spi/Drivers/BNO08X_UART_RVC" -I"C:/Users/HP/workspace_ccstheia/imu660rb-fusion-oled-hardware-spi/Drivers/Ultrasonic_GPIO" -I"C:/Users/HP/workspace_ccstheia/imu660rb-fusion-oled-hardware-spi/Drivers/Ultrasonic_Capture" -I"C:/Users/HP/workspace_ccstheia/imu660rb-fusion-oled-hardware-spi/Drivers/OLED_Hardware_I2C" -I"C:/Users/HP/workspace_ccstheia/imu660rb-fusion-oled-hardware-spi/Drivers/OLED_Hardware_SPI" -I"C:/Users/HP/workspace_ccstheia/imu660rb-fusion-oled-hardware-spi/Drivers/OLED_Software_I2C" -I"C:/Users/HP/workspace_ccstheia/imu660rb-fusion-oled-hardware-spi/Drivers/OLED_Software_SPI" -I"C:/Users/HP/workspace_ccstheia/imu660rb-fusion-oled-hardware-spi/Drivers/MPU6050" -I"C:/Users/HP/workspace_ccstheia/imu660rb-fusion-oled-hardware-spi/Code" -I"C:/Users/HP/workspace_ccstheia/imu660rb-fusion-oled-hardware-spi" -I"C:/Users/HP/workspace_ccstheia/imu660rb-fusion-oled-hardware-spi/Debug" -I"D:/TI_ti/mspm0_sdk_2_05_00_05/source/third_party/CMSIS/Core/Include" -I"D:/TI_ti/mspm0_sdk_2_05_00_05/source" -I"C:/Users/HP/workspace_ccstheia/imu660rb-fusion-oled-hardware-spi/Drivers/MSPM0" -DMOTION_DRIVER_TARGET_MSPM0 -DMPU6050 -D__MSPM0G3507__ -gdwarf-3 -MMD -MP -MF"Drivers/OLED_Software_I2C/$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


