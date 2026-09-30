set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Повні шляхи до інструментів всередині STM32CubeCLT_1.22.0
set(TOOLCHAIN_PREFIX "C:/ST/STM32CubeCLT_1.22.0/GNU-tools-for-STM32/bin")

set(CMAKE_C_COMPILER "${TOOLCHAIN_PREFIX}/arm-none-eabi-gcc.exe")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_PREFIX}/arm-none-eabi-g++.exe")
set(CMAKE_ASM_COMPILER "${TOOLCHAIN_PREFIX}/arm-none-eabi-gcc.exe")
set(CMAKE_OBJCOPY "${TOOLCHAIN_PREFIX}/arm-none-eabi-objcopy.exe")
set(CMAKE_SIZE "${TOOLCHAIN_PREFIX}/arm-none-eabi-size.exe")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)