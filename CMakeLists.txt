cmake_minimum_required(VERSION 2.8)
project( Application )

find_package( OpenCV REQUIRED )
find_package(VTK REQUIRED)
find_package(Iconv REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(PCL 1.8 REQUIRED)

include_directories( ${OpenCV_INCLUDE_DIRS} include/ ${EIGEN3_INCLUDE_DIR} ${PCL_INCLUDE_DIRS} )
include(${VTK_USE_FILE})

# Link directories for PCL
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

file(GLOB SOURCES "src/*.cpp")

add_executable( bin/Application ${SOURCES} )

target_link_libraries( bin/Application ${PCL_LIBRARIES} ${OpenCV_LIBS} ${VTK_LIBRARIES} Iconv::Iconv Eigen3::Eigen )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")