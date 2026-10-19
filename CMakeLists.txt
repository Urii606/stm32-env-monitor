cmake_minimum_required(VERSION 3.22)
project(stm32-env-monitor C CXX ASM)

add_compile_options(-mcpu=cortex-m4 -mthumb)
add_link_options(-mcpu=cortex-m4 -mthumb)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Прапорці мікроконтролера STM32F411 (Cortex-M4F)
set(MCU_FLAGS "-mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb")

set(CMAKE_C_FLAGS "${MCU_FLAGS} -Wall -Wextra -O0 -g3 -DSTM32F411xE" CACHE INTERNAL "C Flags")
set(CMAKE_ASM_FLAGS "${MCU_FLAGS} -x assembler-with-cpp" CACHE INTERNAL "ASM Flags")

# Файл лінкера
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/startup/stm32f411xe_flash.ld)

# Підключаємо сабмодуль із драйверами
add_subdirectory(lib/stm32-drivers)

# Основний виконуваний файл вашого проєкту
add_executable(${PROJECT_NAME}
    App/Src/main.c
    App/Src/logger.c
    startup/startup_stm32f411xe.s
    Devices/Src/buzzer.c
    Devices/Src/dht11.c
    BSP/Src/console.c
    )

# Підключаємо наші власні папки з хедерами (якщо вони є в App/Inc)
target_include_directories(${PROJECT_NAME} PRIVATE
    App/Inc
    Devices/Inc
    BSP/Inc
)

# Лінкуємо статичну бібліотеку драйверів, яку ми створили в сабмодулі
target_link_libraries(${PROJECT_NAME} PRIVATE stm32_drivers)

# Передаємо прапорці лінкеру (кожен з нового рядка, щоб CMake їх не склеював)
target_link_options(${PROJECT_NAME} PRIVATE
    -mcpu=cortex-m4
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -mthumb
    -T${LINKER_SCRIPT}
    --specs=nosys.specs 
    --specs=nano.specs
    -Wl,-Map=${PROJECT_NAME}.map
    -Wl,--gc-sections
)

# Генерація бінарників .bin та .hex
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${PROJECT_NAME} ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME} ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_SIZE} ${PROJECT_NAME}
)


