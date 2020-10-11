cmake_minimum_required(VERSION 3.10)

# required for cross compiler
SET (CMAKE_C_COMPILER_WORKS 1)
SET (CMAKE_CXX_COMPILER_WORKS 1)

project(HryOS C CXX ASM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "-ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti")
set(CMAKE_C_FLAGS "-ffreestanding -O2 -Wall -Wextra")
set(LINKER_SCRIPT "${PROJECT_SOURCE_DIR}/scripts/linker.ld")
set(CMAKE_EXE_LINKER_FLAGS "-T ${LINKER_SCRIPT} -ffreestanding -O2 -nostdlib -lgcc" )

file(GLOB_RECURSE kernel-src
    boot/boot.s
    kernel/*.cpp
    kernel/*.c
    kernel/*.s)

add_executable(HryOS ${kernel-src})

file(COPY ${PROJECT_SOURCE_DIR}/grub.cfg DESTINATION ${PROJECT_SOURCE_DIR}/iso/boot/grub)

add_custom_command(TARGET HryOS POST_BUILD VERBATIM
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:HryOS>" ${PROJECT_SOURCE_DIR}/iso/boot/HryOS.bin)


