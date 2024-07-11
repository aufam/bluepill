set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.10)

# specify cross-compilers and tools
set(CMAKE_SYSTEM_PROCESSOR ARM)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(SIZE arm-none-eabi-size)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# project settings
project(bluepill C CXX ASM)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

# user defines
add_definitions(
    -DF103_USE_CAN                      # since F103 can't use CAN and USB at the same time, we have to choose one
    -DETL_ASYNC_N_CHANNELS=3            # number of tasks that handle asynchronous functions
    -DETL_ASYNC_TASK_THREAD_SIZE=384    # the size of each task
    -DPERIPH_ADC_N_CHANNELS=3           # the NUMBER OF ADC conversion
)

# Enable assembler files preprocessing
add_compile_options($<$<COMPILE_LANGUAGE:ASM>:-x$<SEMICOLON>assembler-with-cpp>)

# software floating point
add_compile_options(-mfloat-abi=soft)

# optimization flags
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message(STATUS "Maximum optimization for speed")
    add_compile_options(-Ofast)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    message(STATUS "Maximum optimization for speed, debug info included")
    add_compile_options(-Ofast -g)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    message(STATUS "Maximum optimization for size")
    add_compile_options(-Os)
else ()
    message(STATUS "Minimal optimization, debug info included")
    add_compile_options(-O2 -ggdb)
endif ()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -fno-exceptions -fno-threadsafe-statics")

# garbage collection
add_compile_options(-ffunction-sections -fdata-sections -fno-common)
add_link_options(-ffunction-sections -fdata-sections -fno-common)

# cpu
add_compile_options(-mcpu=cortex-m3 -mthumb -mthumb-interwork)
add_link_options(-mcpu=cortex-m3 -mthumb -mthumb-interwork)

# warnings
add_compile_options(-Wall -Wextra -Wundef)
add_compile_options(-Wno-double-promotion)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-register -Wno-literal-suffix -Wno-attributes")

# defines
add_definitions(-DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB)

# library
add_link_options(-specs=nano.specs -lm -lc)

# linker script
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F103C8TX_FLASH.ld)
set(MAP ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)
add_link_options(-Wl,-gc-sections,--print-memory-usage,-Map=${MAP},--script=${LINKER_SCRIPT})

# include
include_directories(
	Core/Inc
	Drivers/CMSIS/Device/ST/STM32F1xx/Include
	Drivers/CMSIS/Include
	Drivers/STM32F1xx_HAL_Driver/Inc
	Drivers/STM32F1xx_HAL_Driver/Inc/Legacy
	Middlewares/Third_Party/FreeRTOS/Source/include
	Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2
	Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3
    Middlewares/ST/STM32_USB_Device_Library/Core/Inc
    Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc
    USB_DEVICE/App
    USB_DEVICE/Target
	${CMAKE_HOME_DIRECTORY}
	Project
)

# source files
file(GLOB_RECURSE SOURCES
	Core/*.*
    Middlewares/ST/*.*
    Middlewares/Third_Party/FreeRTOS/*.*
	Drivers/*.*
    USB_DEVICE/*.*
	Project/*.*
)

# build elf
add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

# add etl
add_subdirectory(Middlewares/Third_Party/etl)
target_link_libraries(${PROJECT_NAME}.elf etl)

# add periph
add_subdirectory(Middlewares/Third_Party/stm32_hal_interface)
target_link_libraries(${PROJECT_NAME}.elf periph)

# add oled
add_subdirectory(Middlewares/Third_Party/stm32_oled)
target_link_libraries(${PROJECT_NAME}.elf oled)

# add wizchip
add_subdirectory(Middlewares/Third_Party/stm32_wizchip)
target_link_libraries(${PROJECT_NAME}.elf wizchip)

# add modbus
add_subdirectory(Middlewares/Third_Party/stm32_modbus)
target_link_libraries(${PROJECT_NAME}.elf modbus)

# build hex and bin files
set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
    COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
    COMMENT "Building ${HEX_FILE}\nBuilding ${BIN_FILE}"
)

# flash using st-flash
add_custom_target(flash
    COMMAND st-flash write ${BIN_FILE} 0x08000000
    DEPENDS ${BIN_FILE}
)

# flash using dfu-util
add_custom_target(dfu
    COMMAND dfu-util -a 0 -D ${BIN_FILE} -s 0x08000000:leave
    DEPENDS ${BIN_FILE}
)
