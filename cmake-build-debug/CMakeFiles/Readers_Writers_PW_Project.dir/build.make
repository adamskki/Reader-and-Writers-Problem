# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.12

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
CMAKE_COMMAND = /home/adams/Programy/clion-2018.2.5/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/adams/Programy/clion-2018.2.5/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/adams/CLionProjects/Readers_Writers_PW_Project

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/adams/CLionProjects/Readers_Writers_PW_Project/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/Readers_Writers_PW_Project.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Readers_Writers_PW_Project.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Readers_Writers_PW_Project.dir/flags.make

CMakeFiles/Readers_Writers_PW_Project.dir/main.c.o: CMakeFiles/Readers_Writers_PW_Project.dir/flags.make
CMakeFiles/Readers_Writers_PW_Project.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/adams/CLionProjects/Readers_Writers_PW_Project/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/Readers_Writers_PW_Project.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/Readers_Writers_PW_Project.dir/main.c.o   -c /home/adams/CLionProjects/Readers_Writers_PW_Project/main.c

CMakeFiles/Readers_Writers_PW_Project.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/Readers_Writers_PW_Project.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/adams/CLionProjects/Readers_Writers_PW_Project/main.c > CMakeFiles/Readers_Writers_PW_Project.dir/main.c.i

CMakeFiles/Readers_Writers_PW_Project.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/Readers_Writers_PW_Project.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/adams/CLionProjects/Readers_Writers_PW_Project/main.c -o CMakeFiles/Readers_Writers_PW_Project.dir/main.c.s

# Object files for target Readers_Writers_PW_Project
Readers_Writers_PW_Project_OBJECTS = \
"CMakeFiles/Readers_Writers_PW_Project.dir/main.c.o"

# External object files for target Readers_Writers_PW_Project
Readers_Writers_PW_Project_EXTERNAL_OBJECTS =

Readers_Writers_PW_Project: CMakeFiles/Readers_Writers_PW_Project.dir/main.c.o
Readers_Writers_PW_Project: CMakeFiles/Readers_Writers_PW_Project.dir/build.make
Readers_Writers_PW_Project: CMakeFiles/Readers_Writers_PW_Project.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/adams/CLionProjects/Readers_Writers_PW_Project/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable Readers_Writers_PW_Project"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Readers_Writers_PW_Project.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Readers_Writers_PW_Project.dir/build: Readers_Writers_PW_Project

.PHONY : CMakeFiles/Readers_Writers_PW_Project.dir/build

CMakeFiles/Readers_Writers_PW_Project.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Readers_Writers_PW_Project.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Readers_Writers_PW_Project.dir/clean

CMakeFiles/Readers_Writers_PW_Project.dir/depend:
	cd /home/adams/CLionProjects/Readers_Writers_PW_Project/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/adams/CLionProjects/Readers_Writers_PW_Project /home/adams/CLionProjects/Readers_Writers_PW_Project /home/adams/CLionProjects/Readers_Writers_PW_Project/cmake-build-debug /home/adams/CLionProjects/Readers_Writers_PW_Project/cmake-build-debug /home/adams/CLionProjects/Readers_Writers_PW_Project/cmake-build-debug/CMakeFiles/Readers_Writers_PW_Project.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Readers_Writers_PW_Project.dir/depend
