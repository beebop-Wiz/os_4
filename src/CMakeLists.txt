cmake_minimum_required(VERSION 2.6)
project(FireOS_SRC C)
set(CMAKE_SYSTEM_NAME "Generic")
set(CMAKE_ASM_NASM_FLAGS "-f elf32 -F stabs -g")
set(CMAKE_C_FLAGS "-ffreestanding -nostdlib -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Wno-unused-variable -g")
set(CMAKE_ASM_NASM_OBJECT_FORMAT "elf32")
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/libc/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/libc/lib)
set(CMAKE_C_COMPILER ../gcc-cross/bin/i686-elf-gcc)
set(CMAKE_C_FLAGS "-std=gnu99 -g")
enable_language(ASM_NASM)

# Build the kernel (os_4.bin)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/version.h
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/mkver ${CMAKE_CURRENT_SOURCE_DIR}/version.h
  DEPENDS mkver ${CMAKE_CURRENT_SOURCE_DIR}/version.txt
  COMMENT "Generating version file..."
  )
add_custom_target(version DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/version.h)

add_executable(os_4.bin boot.nasm kernel.c vga.c port.c vgatext.c idt.c idt.nasm real.nasm real.c vbe.c)
add_dependencies(os_4.bin version)

target_link_libraries(os_4.bin "-T linker.ld" -ffreestanding -nostdlib -O2 gcc)

add_custom_target(image ALL ${CMAKE_CURRENT_SOURCE_DIR}/../mkimg DEPENDS os_4.bin)

# Build the C library (libc.a)

# add_library(c STATIC ../libc/src/crt0.s)

# include_directories(libc/include)

# # Build init (init.exe)

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/fs_mount)

# add_executable(init.exe init/init.c)
# add_dependencies(init.exe c)
# add_custom_command(
#   TARGET init.exe
#   POST_BUILD
#   COMMAND /bin/sync
#   COMMENT "Synchronizing file system..."
#   )
# target_link_libraries(init.exe "-L../libc/lib" c)
