# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.2

# Default target executed when no arguments are given to make.
default_target: all
.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/beebop/src/os_4

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/beebop/src/os_4

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "No interactive CMake dialog available..."
	/usr/bin/cmake -E echo No\ interactive\ CMake\ dialog\ available.
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache
.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache
.PHONY : rebuild_cache/fast

# Special rule for the target test
test:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running tests..."
	/usr/bin/ctest --force-new-ctest-process $(ARGS)
.PHONY : test

# Special rule for the target test
test/fast: test
.PHONY : test/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/beebop/src/os_4/CMakeFiles /home/beebop/src/os_4/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/beebop/src/os_4/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean
.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named mkimg

# Build rule for target.
mkimg: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 mkimg
.PHONY : mkimg

# fast build rule for target.
mkimg/fast:
	$(MAKE) -f CMakeFiles/mkimg.dir/build.make CMakeFiles/mkimg.dir/build
.PHONY : mkimg/fast

#=============================================================================
# Target rules for targets named run_test

# Build rule for target.
run_test: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 run_test
.PHONY : run_test

# fast build rule for target.
run_test/fast:
	$(MAKE) -f CMakeFiles/run_test.dir/build.make CMakeFiles/run_test.dir/build
.PHONY : run_test/fast

#=============================================================================
# Target rules for targets named boot2.bin

# Build rule for target.
boot2.bin: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 boot2.bin
.PHONY : boot2.bin

# fast build rule for target.
boot2.bin/fast:
	$(MAKE) -f src/CMakeFiles/boot2.bin.dir/build.make src/CMakeFiles/boot2.bin.dir/build
.PHONY : boot2.bin/fast

#=============================================================================
# Target rules for targets named image

# Build rule for target.
image: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 image
.PHONY : image

# fast build rule for target.
image/fast:
	$(MAKE) -f src/CMakeFiles/image.dir/build.make src/CMakeFiles/image.dir/build
.PHONY : image/fast

#=============================================================================
# Target rules for targets named init.exe

# Build rule for target.
init.exe: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 init.exe
.PHONY : init.exe

# fast build rule for target.
init.exe/fast:
	$(MAKE) -f src/CMakeFiles/init.exe.dir/build.make src/CMakeFiles/init.exe.dir/build
.PHONY : init.exe/fast

#=============================================================================
# Target rules for targets named os_4.bin

# Build rule for target.
os_4.bin: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 os_4.bin
.PHONY : os_4.bin

# fast build rule for target.
os_4.bin/fast:
	$(MAKE) -f src/CMakeFiles/os_4.bin.dir/build.make src/CMakeFiles/os_4.bin.dir/build
.PHONY : os_4.bin/fast

#=============================================================================
# Target rules for targets named version

# Build rule for target.
version: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 version
.PHONY : version

# fast build rule for target.
version/fast:
	$(MAKE) -f src/CMakeFiles/version.dir/build.make src/CMakeFiles/version.dir/build
.PHONY : version/fast

#=============================================================================
# Target rules for targets named doc

# Build rule for target.
doc: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 doc
.PHONY : doc

# fast build rule for target.
doc/fast:
	$(MAKE) -f doc/CMakeFiles/doc.dir/build.make doc/CMakeFiles/doc.dir/build
.PHONY : doc/fast

#=============================================================================
# Target rules for targets named regen_tests

# Build rule for target.
regen_tests: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 regen_tests
.PHONY : regen_tests

# fast build rule for target.
regen_tests/fast:
	$(MAKE) -f tests/CMakeFiles/regen_tests.dir/build.make tests/CMakeFiles/regen_tests.dir/build
.PHONY : regen_tests/fast

#=============================================================================
# Target rules for targets named c

# Build rule for target.
c : cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 c
.PHONY : c

# fast build rule for target.
c/fast:
	$(MAKE) -f libc/CMakeFiles/c.dir/build.make libc/CMakeFiles/c.dir/build
.PHONY : c/fast

mkimg.o: mkimg.c.o
.PHONY : mkimg.o

# target to build an object file
mkimg.c.o:
	$(MAKE) -f CMakeFiles/mkimg.dir/build.make CMakeFiles/mkimg.dir/mkimg.c.o
.PHONY : mkimg.c.o

mkimg.i: mkimg.c.i
.PHONY : mkimg.i

# target to preprocess a source file
mkimg.c.i:
	$(MAKE) -f CMakeFiles/mkimg.dir/build.make CMakeFiles/mkimg.dir/mkimg.c.i
.PHONY : mkimg.c.i

mkimg.s: mkimg.c.s
.PHONY : mkimg.s

# target to generate assembly for a file
mkimg.c.s:
	$(MAKE) -f CMakeFiles/mkimg.dir/build.make CMakeFiles/mkimg.dir/mkimg.c.s
.PHONY : mkimg.c.s

tests/run_test.o: tests/run_test.c.o
.PHONY : tests/run_test.o

# target to build an object file
tests/run_test.c.o:
	$(MAKE) -f CMakeFiles/run_test.dir/build.make CMakeFiles/run_test.dir/tests/run_test.c.o
.PHONY : tests/run_test.c.o

tests/run_test.i: tests/run_test.c.i
.PHONY : tests/run_test.i

# target to preprocess a source file
tests/run_test.c.i:
	$(MAKE) -f CMakeFiles/run_test.dir/build.make CMakeFiles/run_test.dir/tests/run_test.c.i
.PHONY : tests/run_test.c.i

tests/run_test.s: tests/run_test.c.s
.PHONY : tests/run_test.s

# target to generate assembly for a file
tests/run_test.c.s:
	$(MAKE) -f CMakeFiles/run_test.dir/build.make CMakeFiles/run_test.dir/tests/run_test.c.s
.PHONY : tests/run_test.c.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... run_test"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... mkimg"
	@echo "... test"
	@echo "... version"
	@echo "... image"
	@echo "... os_4.bin"
	@echo "... boot2.bin"
	@echo "... init.exe"
	@echo "... doc"
	@echo "... regen_tests"
	@echo "... c"
	@echo "... mkimg.o"
	@echo "... mkimg.i"
	@echo "... mkimg.s"
	@echo "... tests/run_test.o"
	@echo "... tests/run_test.i"
	@echo "... tests/run_test.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

