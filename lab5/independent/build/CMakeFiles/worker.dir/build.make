# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_SOURCE_DIR = /home/wangxv/Files/hw/oshw/lab/lab5/independent

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/wangxv/Files/hw/oshw/lab/lab5/independent/build

# Include any dependencies generated for this target.
include CMakeFiles/worker.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/worker.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/worker.dir/flags.make

CMakeFiles/worker.dir/worker.c.o: CMakeFiles/worker.dir/flags.make
CMakeFiles/worker.dir/worker.c.o: ../worker.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wangxv/Files/hw/oshw/lab/lab5/independent/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/worker.dir/worker.c.o"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/worker.dir/worker.c.o   -c /home/wangxv/Files/hw/oshw/lab/lab5/independent/worker.c

CMakeFiles/worker.dir/worker.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/worker.dir/worker.c.i"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/wangxv/Files/hw/oshw/lab/lab5/independent/worker.c > CMakeFiles/worker.dir/worker.c.i

CMakeFiles/worker.dir/worker.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/worker.dir/worker.c.s"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/wangxv/Files/hw/oshw/lab/lab5/independent/worker.c -o CMakeFiles/worker.dir/worker.c.s

# Object files for target worker
worker_OBJECTS = \
"CMakeFiles/worker.dir/worker.c.o"

# External object files for target worker
worker_EXTERNAL_OBJECTS =

worker: CMakeFiles/worker.dir/worker.c.o
worker: CMakeFiles/worker.dir/build.make
worker: CMakeFiles/worker.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/wangxv/Files/hw/oshw/lab/lab5/independent/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable worker"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/worker.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/worker.dir/build: worker

.PHONY : CMakeFiles/worker.dir/build

CMakeFiles/worker.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/worker.dir/cmake_clean.cmake
.PHONY : CMakeFiles/worker.dir/clean

CMakeFiles/worker.dir/depend:
	cd /home/wangxv/Files/hw/oshw/lab/lab5/independent/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/wangxv/Files/hw/oshw/lab/lab5/independent /home/wangxv/Files/hw/oshw/lab/lab5/independent /home/wangxv/Files/hw/oshw/lab/lab5/independent/build /home/wangxv/Files/hw/oshw/lab/lab5/independent/build /home/wangxv/Files/hw/oshw/lab/lab5/independent/build/CMakeFiles/worker.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/worker.dir/depend

