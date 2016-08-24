cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0015 OLD)
cmake_policy(SET CMP0046 OLD)
project(FireOS_SRC C)
set(CMAKE_SYSTEM_NAME "Generic")
set(CMAKE_ASM_NASM_FLAGS "-f elf32 -F stabs -g")
set(CMAKE_ASM_NASM_OBJECT_FORMAT "elf32")
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/libc/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/libc/lib)
set(CMAKE_C_COMPILER ../gcc-cross/bin/i686-elf-gcc)
set(CMAKE_C_FLAGS "-ffreestanding -fno-builtin -nostdlib -Wall -Wextra -Werror -std=gnu99 -g")
enable_language(ASM_NASM)

# Build the second stage bootstrap (boot2.bin)

add_executable(boot2.bin boot2.c vga.c vgatext.c boot.nasm port.c vbe.c real.nasm real.c vgadraw.c idt.c idt.nasm log.c)
target_link_libraries(boot2.bin "-T boot2.ld" -ffreestanding -nostdlib -O2 gcc)

# Build the kernel (os_4.bin)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/version.h
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/mkver ${CMAKE_CURRENT_SOURCE_DIR}/version.h
  DEPENDS mkver ${CMAKE_CURRENT_SOURCE_DIR}/version.txt
  COMMENT "Generating version file..."
  )
add_custom_target(version DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/version.h)

add_executable(os_4.bin kernel.c vga.c port.c vgatext.c idt.c idt.nasm real.nasm real.c vbe.c malloc.c vgadraw.c paging.c debug.c gdt.c user.nasm ring3.c syscall.c timer.c mt.c util.c ext2.c keyboard.c pic.c async.c log.c fs/fs.c fs/devfs.c )
add_dependencies(os_4.bin version)

target_link_libraries(os_4.bin "-T linker.ld" -ffreestanding -nostdlib -O2 gcc)
set_target_properties(os_4.bin PROPERTIES COMPILE_FLAGS "-DKERNEL_SOURCE")

add_custom_target(image ALL ${CMAKE_CURRENT_SOURCE_DIR}/../mkimg DEPENDS os_4.bin)

# Build init (init.exe)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../fs_mount/boot/)
link_directories(../libc/lib/)
include_directories(../libc/include/)
add_executable(init.exe init/init.c)
add_custom_command(
   TARGET init.exe
   POST_BUILD
   COMMAND /bin/sync
   COMMENT "Synchronizing file system..."
   )
add_dependencies(init.exe ../libc/lib/libc.a)
target_link_libraries(init.exe c)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../fs_mount/bin/)
add_executable(hello init/hello.c)
target_link_libraries(hello c)
#add_executable(env coreutils/env.c)
#target_link_libraries(env c)
add_executable(echo coreutils/echo.c)
target_link_libraries(echo c)
add_executable(cat coreutils/cat.c)
target_link_libraries(cat c)
add_executable(sh coreutils/sh.c)
target_link_libraries(sh c)
add_custom_target(coreutils ALL DEPENDS hello)

add_custom_command(
  TARGET coreutils
  POST_BUILD
  COMMAND /bin/sync
  COMMENT "Synchronizing file system..."
  )
