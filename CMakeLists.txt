cmake_minimum_required(VERSION 3.22)
project(stm32-env-monitor C CXX ASM)

# Set base CPU and instruction set target for compiler and linker
add_compile_options(-mcpu=cortex-m4 -mthumb)
add_link_options(-mcpu=cortex-m4 -mthumb)

# Generate compile_commands.json to support language server navigation (clangd, VSCode)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable single-precision hardware FPU and hard-float ABI for Cortex-M4F
set(MCU_FLAGS "-mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb")

# Select build flags based on CMAKE_BUILD_TYPE configuration
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Disable optimization (-O0) and include max debug symbols (-g3) for interactive debugging
    set(CMAKE_C_FLAGS "${MCU_FLAGS} -Wall -Wextra -O0 -g3 -DSTM32F411xE")
else()
    # Optimize for code size (-Os) in production builds
    set(CMAKE_C_FLAGS "${MCU_FLAGS} -Wall -Wextra -Os -DSTM32F411xE")
endif()

# Specify memory layout script for the linker
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/startup/stm32f411xe_flash.ld)

# Include hardware abstraction driver library subproject
add_subdirectory(lib/stm32-drivers)

# Define project executable and its target source files
add_executable(${PROJECT_NAME}
    App/Src/main.c
    App/Src/logger.c
    startup/startup_stm32f411xe.s
    Devices/Src/buzzer.c
    Devices/Src/dht11.c
    Devices/Src/lcd1602.c
    BSP/Src/console.c
    BSP/Src/bsp.c
)

# Conditionally pass DEBUG preprocessor macro only for Debug target builds
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
endif()

# Include paths for application, device drivers, and BSP headers
target_include_directories(${PROJECT_NAME} PRIVATE
    App/Inc
    Devices/Inc
    BSP/Inc
)

# Link static driver library
target_link_libraries(${PROJECT_NAME} PRIVATE stm32_drivers)

# Configure low-level linker options and standard library specs
target_link_options(${PROJECT_NAME} PRIVATE
    -mcpu=cortex-m4
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -mthumb
    -T${LINKER_SCRIPT}
    --specs=nosys.specs 
    --specs=nano.specs            # Use newlib-nano to minimize SRAM footprint
    -Wl,-Map=${PROJECT_NAME}.map  # Generate memory map file for RAM/Flash analysis
    -Wl,--gc-sections             # Remove unreferenced functions/data to save Flash
)

# Post-build step: convert ELF output into flashable binaries and print RAM/Flash usage
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${PROJECT_NAME} ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME} ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_SIZE} ${PROJECT_NAME}
)