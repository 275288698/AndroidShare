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
include math/CMakeFiles/MFun.dir/depend.make

# Include the progress variables for this target.
include math/CMakeFiles/MFun.dir/progress.make

# Include the compile flags for this target's objects.
include math/CMakeFiles/MFun.dir/flags.make

math/CMakeFiles/MFun.dir/MFun.cc.o: math/CMakeFiles/MFun.dir/flags.make
math/CMakeFiles/MFun.dir/MFun.cc.o: math/MFun.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object math/CMakeFiles/MFun.dir/MFun.cc.o"
	cd /Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo/math && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/MFun.dir/MFun.cc.o -c /Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo/math/MFun.cc

math/CMakeFiles/MFun.dir/MFun.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MFun.dir/MFun.cc.i"
	cd /Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo/math && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo/math/MFun.cc > CMakeFiles/MFun.dir/MFun.cc.i

math/CMakeFiles/MFun.dir/MFun.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MFun.dir/MFun.cc.s"
	cd /Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo/math && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo/math/MFun.cc -o CMakeFiles/MFun.dir/MFun.cc.s

math/CMakeFiles/MFun.dir/MFun.cc.o.requires:

.PHONY : math/CMakeFiles/MFun.dir/MFun.cc.o.requires

math/CMakeFiles/MFun.dir/MFun.cc.o.provides: math/CMakeFiles/MFun.dir/MFun.cc.o.requires
	$(MAKE) -f math/CMakeFiles/MFun.dir/build.make math/CMakeFiles/MFun.dir/MFun.cc.o.provides.build
.PHONY : math/CMakeFiles/MFun.dir/MFun.cc.o.provides

math/CMakeFiles/MFun.dir/MFun.cc.o.provides.build: math/CMakeFiles/MFun.dir/MFun.cc.o


# Object files for target MFun
MFun_OBJECTS = \
"CMakeFiles/MFun.dir/MFun.cc.o"

# External object files for target MFun
MFun_EXTERNAL_OBJECTS =

math/libMFun.a: math/CMakeFiles/MFun.dir/MFun.cc.o
math/libMFun.a: math/CMakeFiles/MFun.dir/build.make
math/libMFun.a: math/CMakeFiles/MFun.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libMFun.a"
	cd /Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo/math && $(CMAKE_COMMAND) -P CMakeFiles/MFun.dir/cmake_clean_target.cmake
	cd /Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo/math && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/MFun.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
math/CMakeFiles/MFun.dir/build: math/libMFun.a

.PHONY : math/CMakeFiles/MFun.dir/build

math/CMakeFiles/MFun.dir/requires: math/CMakeFiles/MFun.dir/MFun.cc.o.requires

.PHONY : math/CMakeFiles/MFun.dir/requires

math/CMakeFiles/MFun.dir/clean:
	cd /Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo/math && $(CMAKE_COMMAND) -P CMakeFiles/MFun.dir/cmake_clean.cmake
.PHONY : math/CMakeFiles/MFun.dir/clean

math/CMakeFiles/MFun.dir/depend:
	cd /Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo /Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo/math /Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo /Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo/math /Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo/math/CMakeFiles/MFun.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : math/CMakeFiles/MFun.dir/depend

