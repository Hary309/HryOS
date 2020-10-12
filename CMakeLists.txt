cmake_minimum_required(VERSION 3.10)

# required for cross compiler
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

project(HryOS C CXX ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffreestanding -Wall -Wextra -fno-exceptions -fno-rtti")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffreestanding -Wall -Wextra")
set(LINKER_SCRIPT "${PROJECT_SOURCE_DIR}/scripts/linker.ld")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${LINKER_SCRIPT} -ffreestanding -nostdlib -lgcc" )

file(GLOB_RECURSE kernel-src
    src/kernel/*.cpp
    src/kernel/*.c
    src/kernel/*.s)

add_executable(HryOS ${kernel-src})

target_include_directories(HryOS PRIVATE src/kernel)

file(COPY ${PROJECT_SOURCE_DIR}/config/grub.cfg DESTINATION ${PROJECT_SOURCE_DIR}/iso/boot/grub)

add_custom_command(TARGET HryOS POST_BUILD VERBATIM
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:HryOS>" ${PROJECT_SOURCE_DIR}/iso/boot/HryOS.bin)
