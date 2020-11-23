cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_VERBOSE_MAKEFILE ON)

# required for cross compiler
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

project(HryOS C CXX ASM)
enable_language(ASM_NASM)

if(NOT CMAKE_ASM_NASM_COMPILER_LOADED)
  message("Cannot find NASM compiler!")
endif(NOT CMAKE_ASM_NASM_COMPILER_LOADED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -ffreestanding -Wall -Wextra -fno-exceptions -fno-rtti")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffreestanding -Wall -Wextra")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -felf32")
set(LINKER_SCRIPT "${PROJECT_SOURCE_DIR}/scripts/linker.ld")
set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -T ${LINKER_SCRIPT} -ffreestanding -nostdlib -lgcc"
)

include(hlib.cmake)

file(GLOB_RECURSE kernel-src code/kernel/*.cpp code/kernel/*.c
     code/kernel/*.asm)

add_executable(HryOS ${kernel-src})

target_include_directories(HryOS PRIVATE code/kernel)
target_link_libraries(HryOS PRIVATE hlib)

file(COPY ${PROJECT_SOURCE_DIR}/config/grub.cfg
     DESTINATION ${PROJECT_SOURCE_DIR}/iso/boot/grub)

add_custom_command(
  TARGET HryOS
  POST_BUILD VERBATIM
  COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:HryOS>"
          ${PROJECT_SOURCE_DIR}/iso/boot/HryOS.bin)
