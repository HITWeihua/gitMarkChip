cmake_minimum_required(VERSION 2.8)

SET(CMAKE_BUILD_TYPE "Debug")  
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")  
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")  

project( LabelMaker )
find_package( OpenCV REQUIRED )
#头文件目录
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( label_marker main.cpp )
target_link_libraries( label_marker ${OpenCV_LIBS} )
target_link_libraries( label_marker /lib/x86_64-linux-gnu/libreadline.so.6)
