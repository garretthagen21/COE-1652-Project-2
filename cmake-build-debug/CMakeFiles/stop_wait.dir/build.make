# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.14

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
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/Users/GarrettHagen 1/Development/C++/COE-1652/COE-1652-Project-2"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/Users/GarrettHagen 1/Development/C++/COE-1652/COE-1652-Project-2/cmake-build-debug"

# Include any dependencies generated for this target.
include CMakeFiles/stop_wait.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/stop_wait.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/stop_wait.dir/flags.make

CMakeFiles/stop_wait.dir/project2_stop_wait.c.o: CMakeFiles/stop_wait.dir/flags.make
CMakeFiles/stop_wait.dir/project2_stop_wait.c.o: ../project2_stop_wait.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/Users/GarrettHagen 1/Development/C++/COE-1652/COE-1652-Project-2/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/stop_wait.dir/project2_stop_wait.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/stop_wait.dir/project2_stop_wait.c.o   -c "/Users/GarrettHagen 1/Development/C++/COE-1652/COE-1652-Project-2/project2_stop_wait.c"

CMakeFiles/stop_wait.dir/project2_stop_wait.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/stop_wait.dir/project2_stop_wait.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/Users/GarrettHagen 1/Development/C++/COE-1652/COE-1652-Project-2/project2_stop_wait.c" > CMakeFiles/stop_wait.dir/project2_stop_wait.c.i

CMakeFiles/stop_wait.dir/project2_stop_wait.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/stop_wait.dir/project2_stop_wait.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/Users/GarrettHagen 1/Development/C++/COE-1652/COE-1652-Project-2/project2_stop_wait.c" -o CMakeFiles/stop_wait.dir/project2_stop_wait.c.s

# Object files for target stop_wait
stop_wait_OBJECTS = \
"CMakeFiles/stop_wait.dir/project2_stop_wait.c.o"

# External object files for target stop_wait
stop_wait_EXTERNAL_OBJECTS =

stop_wait: CMakeFiles/stop_wait.dir/project2_stop_wait.c.o
stop_wait: CMakeFiles/stop_wait.dir/build.make
stop_wait: CMakeFiles/stop_wait.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/Users/GarrettHagen 1/Development/C++/COE-1652/COE-1652-Project-2/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable stop_wait"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/stop_wait.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/stop_wait.dir/build: stop_wait

.PHONY : CMakeFiles/stop_wait.dir/build

CMakeFiles/stop_wait.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/stop_wait.dir/cmake_clean.cmake
.PHONY : CMakeFiles/stop_wait.dir/clean

CMakeFiles/stop_wait.dir/depend:
	cd "/Users/GarrettHagen 1/Development/C++/COE-1652/COE-1652-Project-2/cmake-build-debug" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/Users/GarrettHagen 1/Development/C++/COE-1652/COE-1652-Project-2" "/Users/GarrettHagen 1/Development/C++/COE-1652/COE-1652-Project-2" "/Users/GarrettHagen 1/Development/C++/COE-1652/COE-1652-Project-2/cmake-build-debug" "/Users/GarrettHagen 1/Development/C++/COE-1652/COE-1652-Project-2/cmake-build-debug" "/Users/GarrettHagen 1/Development/C++/COE-1652/COE-1652-Project-2/cmake-build-debug/CMakeFiles/stop_wait.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/stop_wait.dir/depend

