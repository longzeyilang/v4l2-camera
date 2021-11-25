cmake_minimum_required(VERSION 3.0)
project(camera_demo)
set(CMAKE_BUILD_TYPE "Release")
# set (CMAKE_CXX_STANDARD 11)
file(GLOB srcs "v4l2.c  camera_demo.cpp")
file(GLOB hdrs ${CMAKE_CURRENT_SOURCE_DIR})

# opencv
find_package(OpenCV)
message("OpenCV Headers: ${OpenCV_LIBRARIES}\n")

include_directories(${OpenCV_INCLUDE_DIRS} )
include_directories(${hdrs})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(camera_demo  camera_demo.cpp v4l2.c camera.c)

#target_link_libraries(camera_demo  ${OpenCV_LIBRARIES} /lib/x86_64-linux-gnu/libpthread.so.0)

target_link_libraries(camera_demo  ${OpenCV_LIBRARIES} /lib/aarch64-linux-gnu/libpthread.so.0)