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
include CMakeFiles/enter.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/enter.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/enter.dir/flags.make

CMakeFiles/enter.dir/enter.c.o: CMakeFiles/enter.dir/flags.make
CMakeFiles/enter.dir/enter.c.o: ../enter.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wangxv/Files/hw/oshw/lab/lab5/independent/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/enter.dir/enter.c.o"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/enter.dir/enter.c.o   -c /home/wangxv/Files/hw/oshw/lab/lab5/independent/enter.c

CMakeFiles/enter.dir/enter.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/enter.dir/enter.c.i"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/wangxv/Files/hw/oshw/lab/lab5/independent/enter.c > CMakeFiles/enter.dir/enter.c.i

CMakeFiles/enter.dir/enter.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/enter.dir/enter.c.s"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/wangxv/Files/hw/oshw/lab/lab5/independent/enter.c -o CMakeFiles/enter.dir/enter.c.s

# Object files for target enter
enter_OBJECTS = \
"CMakeFiles/enter.dir/enter.c.o"

# External object files for target enter
enter_EXTERNAL_OBJECTS =

enter: CMakeFiles/enter.dir/enter.c.o
enter: CMakeFiles/enter.dir/build.make
enter: CMakeFiles/enter.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/wangxv/Files/hw/oshw/lab/lab5/independent/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable enter"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/enter.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/enter.dir/build: enter

.PHONY : CMakeFiles/enter.dir/build

CMakeFiles/enter.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/enter.dir/cmake_clean.cmake
.PHONY : CMakeFiles/enter.dir/clean

CMakeFiles/enter.dir/depend:
	cd /home/wangxv/Files/hw/oshw/lab/lab5/independent/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/wangxv/Files/hw/oshw/lab/lab5/independent /home/wangxv/Files/hw/oshw/lab/lab5/independent /home/wangxv/Files/hw/oshw/lab/lab5/independent/build /home/wangxv/Files/hw/oshw/lab/lab5/independent/build /home/wangxv/Files/hw/oshw/lab/lab5/independent/build/CMakeFiles/enter.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/enter.dir/depend
