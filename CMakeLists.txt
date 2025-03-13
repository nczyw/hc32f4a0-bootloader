cmake_minimum_required(VERSION 3.27)


# Setup compiler settings
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_PROJECT_NAME hc32f4a0-bootloader VERSION 1.0.0)

set(MCU_TYPE "HC32F4A0xI" CACHE STRING "Set MCU Type")
set(BOOTLOADER ON CACHE BOOL "Set to ON if bootloader is enabled")
set(APP OFF CACHE BOOL "Set to ON if app is enabled")
set(RT-THREAD OFF CACHE BOOL "Set to ON if RT-Thread is enabled")

# Include toolchain file
include("drivers/toolchain/gcc-arm-none-eabi.cmake")

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})
# "Generate hex and bin files and associate them with the clean target."
add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME}
    POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.hex
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.bin
    BYPRODUCTS 
        ${CMAKE_PROJECT_NAME}.hex
        ${CMAKE_PROJECT_NAME}.bin
)
add_subdirectory("drivers")
add_subdirectory("fatfs")

file(GLOB SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/hardware/src/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/iap/src/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/src/*.c"
)
set(HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/hardware/inc"
    "${CMAKE_CURRENT_LIST_DIR}/iap/inc"
    "${CMAKE_CURRENT_LIST_DIR}/inc"
)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    "${SOURCES}"
)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    "${HEADERS}"
)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
)
