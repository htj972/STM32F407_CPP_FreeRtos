# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.25

# compile ASM with E:/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-gcc.exe
# compile C with E:/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-gcc.exe
# compile CXX with E:/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-g++.exe
ASM_DEFINES = -DARM_MATH_CM4 -DARM_MATH_MATRIX_CHECK -DARM_MATH_ROUNDING -DSTM32F40_41xxx -DUSE_STDPERIPH_DRIVER -D__FPU_PRESENT=1

ASM_INCLUDES = -IF:\code\STM32\STM32F407_CPP_FreeRtos\Core\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\FWLIB\inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\SYSTEM\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\USER\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\HARDWARE\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\FATFS\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\FreeRTOS\FreeRTOS_Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\MIDDLEWARE\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\MIDDLEWARE\BASE_CLASS\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\HARDWARE\BASE_CLASS\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\SOFTWARE\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\USB\USB_HOST\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\USB\USB_OTG\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\USB\CLASS\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\LWIP\lwip-1.4.1\src\include -IF:\code\STM32\STM32F407_CPP_FreeRtos\LWIP\lwip-1.4.1\src\include\ipv4 -IF:\code\STM32\STM32F407_CPP_FreeRtos\LWIP\arch -IF:\code\STM32\STM32F407_CPP_FreeRtos\LWIP\lwip_app

ASM_FLAGS = -g

C_DEFINES = -DARM_MATH_CM4 -DARM_MATH_MATRIX_CHECK -DARM_MATH_ROUNDING -DSTM32F40_41xxx -DUSE_STDPERIPH_DRIVER -D__FPU_PRESENT=1

C_INCLUDES = -IF:\code\STM32\STM32F407_CPP_FreeRtos\Core\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\FWLIB\inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\SYSTEM\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\USER\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\HARDWARE\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\FATFS\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\FreeRTOS\FreeRTOS_Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\MIDDLEWARE\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\MIDDLEWARE\BASE_CLASS\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\HARDWARE\BASE_CLASS\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\SOFTWARE\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\USB\USB_HOST\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\USB\USB_OTG\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\USB\CLASS\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\LWIP\lwip-1.4.1\src\include -IF:\code\STM32\STM32F407_CPP_FreeRtos\LWIP\lwip-1.4.1\src\include\ipv4 -IF:\code\STM32\STM32F407_CPP_FreeRtos\LWIP\arch -IF:\code\STM32\STM32F407_CPP_FreeRtos\LWIP\lwip_app

C_FLAGS = -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mthumb -mthumb-interwork -ffunction-sections -fdata-sections     -g -fno-common -fmessage-length=0 -specs=nosys.specs -specs=nano.specs -u _printf_float -std=gnu99 -g -fdiagnostics-color=always

CXX_DEFINES = -DARM_MATH_CM4 -DARM_MATH_MATRIX_CHECK -DARM_MATH_ROUNDING -DSTM32F40_41xxx -DUSE_STDPERIPH_DRIVER -D__FPU_PRESENT=1

CXX_INCLUDES = -IF:\code\STM32\STM32F407_CPP_FreeRtos\Core\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\FWLIB\inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\SYSTEM\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\USER\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\HARDWARE\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\FATFS\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\FreeRTOS\FreeRTOS_Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\MIDDLEWARE\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\MIDDLEWARE\BASE_CLASS\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\HARDWARE\BASE_CLASS\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\SOFTWARE\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\USB\USB_HOST\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\USB\USB_OTG\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\USB\CLASS\Inc -IF:\code\STM32\STM32F407_CPP_FreeRtos\LWIP\lwip-1.4.1\src\include -IF:\code\STM32\STM32F407_CPP_FreeRtos\LWIP\lwip-1.4.1\src\include\ipv4 -IF:\code\STM32\STM32F407_CPP_FreeRtos\LWIP\arch -IF:\code\STM32\STM32F407_CPP_FreeRtos\LWIP\lwip_app

CXX_FLAGS = -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mthumb -mthumb-interwork -ffunction-sections -fdata-sections     -g -fno-common -fmessage-length=0 -specs=nosys.specs -specs=nano.specs -u _printf_float -std=c++11 -g -fdiagnostics-color=always -std=gnu++11

