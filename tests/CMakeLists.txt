cmake_minimum_required(VERSION 2.6)
project(TEST C)
set(CMAKE_SYSTEM_NAME "Generic")
set(CMAKE_ASM_NASM_FLAGS "-f elf32 -F stabs -g")
set(CMAKE_C_FLAGS "-ffreestanding -nostdlib -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Wno-unused-variable -g")
set(CMAKE_ASM_NASM_OBJECT_FORMAT "elf32")
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/libc/lib)
set(CMAKE_C_COMPILER ../gcc-cross/bin/i686-elf-gcc)
enable_language(ASM_NASM)

include_directories(../src/libc/include ../src)

set(KERNEL_SRCS boot.nasm kernel.c vga.c port.c vgatext.c)

add_custom_target(regen_tests ALL perl regen_tests.pl
  SOURCES regen_tests.pl
  )

include(cmake_tests.txt)
