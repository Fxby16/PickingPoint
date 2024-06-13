# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/opencv-4.x

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/build

# Include any dependencies generated for this target.
include modules/structured_light/CMakeFiles/opencv_structured_light.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include modules/structured_light/CMakeFiles/opencv_structured_light.dir/compiler_depend.make

# Include the progress variables for this target.
include modules/structured_light/CMakeFiles/opencv_structured_light.dir/progress.make

# Include the compile flags for this target's objects.
include modules/structured_light/CMakeFiles/opencv_structured_light.dir/flags.make

modules/structured_light/CMakeFiles/opencv_structured_light.dir/src/graycodepattern.cpp.o: modules/structured_light/CMakeFiles/opencv_structured_light.dir/flags.make
modules/structured_light/CMakeFiles/opencv_structured_light.dir/src/graycodepattern.cpp.o: /mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/opencv_contrib-4.x/modules/structured_light/src/graycodepattern.cpp
modules/structured_light/CMakeFiles/opencv_structured_light.dir/src/graycodepattern.cpp.o: modules/structured_light/CMakeFiles/opencv_structured_light.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object modules/structured_light/CMakeFiles/opencv_structured_light.dir/src/graycodepattern.cpp.o"
	cd /mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/build/modules/structured_light && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT modules/structured_light/CMakeFiles/opencv_structured_light.dir/src/graycodepattern.cpp.o -MF CMakeFiles/opencv_structured_light.dir/src/graycodepattern.cpp.o.d -o CMakeFiles/opencv_structured_light.dir/src/graycodepattern.cpp.o -c /mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/opencv_contrib-4.x/modules/structured_light/src/graycodepattern.cpp

modules/structured_light/CMakeFiles/opencv_structured_light.dir/src/graycodepattern.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/opencv_structured_light.dir/src/graycodepattern.cpp.i"
	cd /mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/build/modules/structured_light && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/opencv_contrib-4.x/modules/structured_light/src/graycodepattern.cpp > CMakeFiles/opencv_structured_light.dir/src/graycodepattern.cpp.i

modules/structured_light/CMakeFiles/opencv_structured_light.dir/src/graycodepattern.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/opencv_structured_light.dir/src/graycodepattern.cpp.s"
	cd /mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/build/modules/structured_light && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/opencv_contrib-4.x/modules/structured_light/src/graycodepattern.cpp -o CMakeFiles/opencv_structured_light.dir/src/graycodepattern.cpp.s

modules/structured_light/CMakeFiles/opencv_structured_light.dir/src/sinusoidalpattern.cpp.o: modules/structured_light/CMakeFiles/opencv_structured_light.dir/flags.make
modules/structured_light/CMakeFiles/opencv_structured_light.dir/src/sinusoidalpattern.cpp.o: /mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/opencv_contrib-4.x/modules/structured_light/src/sinusoidalpattern.cpp
modules/structured_light/CMakeFiles/opencv_structured_light.dir/src/sinusoidalpattern.cpp.o: modules/structured_light/CMakeFiles/opencv_structured_light.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object modules/structured_light/CMakeFiles/opencv_structured_light.dir/src/sinusoidalpattern.cpp.o"
	cd /mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/build/modules/structured_light && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT modules/structured_light/CMakeFiles/opencv_structured_light.dir/src/sinusoidalpattern.cpp.o -MF CMakeFiles/opencv_structured_light.dir/src/sinusoidalpattern.cpp.o.d -o CMakeFiles/opencv_structured_light.dir/src/sinusoidalpattern.cpp.o -c /mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/opencv_contrib-4.x/modules/structured_light/src/sinusoidalpattern.cpp

modules/structured_light/CMakeFiles/opencv_structured_light.dir/src/sinusoidalpattern.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/opencv_structured_light.dir/src/sinusoidalpattern.cpp.i"
	cd /mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/build/modules/structured_light && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/opencv_contrib-4.x/modules/structured_light/src/sinusoidalpattern.cpp > CMakeFiles/opencv_structured_light.dir/src/sinusoidalpattern.cpp.i

modules/structured_light/CMakeFiles/opencv_structured_light.dir/src/sinusoidalpattern.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/opencv_structured_light.dir/src/sinusoidalpattern.cpp.s"
	cd /mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/build/modules/structured_light && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/opencv_contrib-4.x/modules/structured_light/src/sinusoidalpattern.cpp -o CMakeFiles/opencv_structured_light.dir/src/sinusoidalpattern.cpp.s

# Object files for target opencv_structured_light
opencv_structured_light_OBJECTS = \
"CMakeFiles/opencv_structured_light.dir/src/graycodepattern.cpp.o" \
"CMakeFiles/opencv_structured_light.dir/src/sinusoidalpattern.cpp.o"

# External object files for target opencv_structured_light
opencv_structured_light_EXTERNAL_OBJECTS =

lib/libopencv_structured_light.so.4.10.0: modules/structured_light/CMakeFiles/opencv_structured_light.dir/src/graycodepattern.cpp.o
lib/libopencv_structured_light.so.4.10.0: modules/structured_light/CMakeFiles/opencv_structured_light.dir/src/sinusoidalpattern.cpp.o
lib/libopencv_structured_light.so.4.10.0: modules/structured_light/CMakeFiles/opencv_structured_light.dir/build.make
lib/libopencv_structured_light.so.4.10.0: lib/libopencv_phase_unwrapping.so.4.10.0
lib/libopencv_structured_light.so.4.10.0: lib/libopencv_calib3d.so.4.10.0
lib/libopencv_structured_light.so.4.10.0: 3rdparty/lib/libippiw.a
lib/libopencv_structured_light.so.4.10.0: 3rdparty/ippicv/ippicv_lnx/icv/lib/intel64/libippicv.a
lib/libopencv_structured_light.so.4.10.0: lib/libopencv_features2d.so.4.10.0
lib/libopencv_structured_light.so.4.10.0: lib/libopencv_flann.so.4.10.0
lib/libopencv_structured_light.so.4.10.0: lib/libopencv_imgproc.so.4.10.0
lib/libopencv_structured_light.so.4.10.0: lib/libopencv_core.so.4.10.0
lib/libopencv_structured_light.so.4.10.0: modules/structured_light/CMakeFiles/opencv_structured_light.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX shared library ../../lib/libopencv_structured_light.so"
	cd /mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/build/modules/structured_light && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/opencv_structured_light.dir/link.txt --verbose=$(VERBOSE)
	cd /mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/build/modules/structured_light && $(CMAKE_COMMAND) -E cmake_symlink_library ../../lib/libopencv_structured_light.so.4.10.0 ../../lib/libopencv_structured_light.so.410 ../../lib/libopencv_structured_light.so

lib/libopencv_structured_light.so.410: lib/libopencv_structured_light.so.4.10.0
	@$(CMAKE_COMMAND) -E touch_nocreate lib/libopencv_structured_light.so.410

lib/libopencv_structured_light.so: lib/libopencv_structured_light.so.4.10.0
	@$(CMAKE_COMMAND) -E touch_nocreate lib/libopencv_structured_light.so

# Rule to build all files generated by this target.
modules/structured_light/CMakeFiles/opencv_structured_light.dir/build: lib/libopencv_structured_light.so
.PHONY : modules/structured_light/CMakeFiles/opencv_structured_light.dir/build

modules/structured_light/CMakeFiles/opencv_structured_light.dir/clean:
	cd /mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/build/modules/structured_light && $(CMAKE_COMMAND) -P CMakeFiles/opencv_structured_light.dir/cmake_clean.cmake
.PHONY : modules/structured_light/CMakeFiles/opencv_structured_light.dir/clean

modules/structured_light/CMakeFiles/opencv_structured_light.dir/depend:
	cd /mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/opencv-4.x /mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/opencv_contrib-4.x/modules/structured_light /mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/build /mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/build/modules/structured_light /mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/build/modules/structured_light/CMakeFiles/opencv_structured_light.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : modules/structured_light/CMakeFiles/opencv_structured_light.dir/depend
