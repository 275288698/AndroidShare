# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.9

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
CMAKE_COMMAND = /Applications/CMake.app/Contents/bin/cmake

# The command to remove a file.
RM = /Applications/CMake.app/Contents/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo

# Include any dependencies generated for this target.
include CMakeFiles/Demo_.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Demo_.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Demo_.dir/flags.make

CMakeFiles/Demo_.dir/main.cc.o: CMakeFiles/Demo_.dir/flags.make
CMakeFiles/Demo_.dir/main.cc.o: main.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Demo_.dir/main.cc.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Demo_.dir/main.cc.o -c /Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo/main.cc

CMakeFiles/Demo_.dir/main.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Demo_.dir/main.cc.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo/main.cc > CMakeFiles/Demo_.dir/main.cc.i

CMakeFiles/Demo_.dir/main.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Demo_.dir/main.cc.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo/main.cc -o CMakeFiles/Demo_.dir/main.cc.s

CMakeFiles/Demo_.dir/main.cc.o.requires:

.PHONY : CMakeFiles/Demo_.dir/main.cc.o.requires

CMakeFiles/Demo_.dir/main.cc.o.provides: CMakeFiles/Demo_.dir/main.cc.o.requires
	$(MAKE) -f CMakeFiles/Demo_.dir/build.make CMakeFiles/Demo_.dir/main.cc.o.provides.build
.PHONY : CMakeFiles/Demo_.dir/main.cc.o.provides

CMakeFiles/Demo_.dir/main.cc.o.provides.build: CMakeFiles/Demo_.dir/main.cc.o


# Object files for target Demo_
Demo__OBJECTS = \
"CMakeFiles/Demo_.dir/main.cc.o"

# External object files for target Demo_
Demo__EXTERNAL_OBJECTS =

Demo_: CMakeFiles/Demo_.dir/main.cc.o
Demo_: CMakeFiles/Demo_.dir/build.make
Demo_: CMakeFiles/Demo_.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Demo_"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Demo_.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Demo_.dir/build: Demo_

.PHONY : CMakeFiles/Demo_.dir/build

CMakeFiles/Demo_.dir/requires: CMakeFiles/Demo_.dir/main.cc.o.requires

.PHONY : CMakeFiles/Demo_.dir/requires

CMakeFiles/Demo_.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Demo_.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Demo_.dir/clean

CMakeFiles/Demo_.dir/depend:
	cd /Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo /Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo /Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo /Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo /Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo/CMakeFiles/Demo_.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Demo_.dir/depend

