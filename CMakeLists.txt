cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(HryOS)

set(LINKER_SCRIPT "scripts/linker.ld")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${LINKER_SCRIPT}")

file(HryOS::boot-src boot/boot.s)
file(HryOS::kernel-src
    kernel/kernel.cpp)
