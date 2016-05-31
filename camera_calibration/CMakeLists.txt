cmake_minimum_required(VERSION 3.5)
project(cv_calib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES camera_calibration.cpp)
add_executable(cv_calib ${SOURCE_FILES})

find_package(OpenCV REQUIRED)
include_directories("${OpenCV_INCLUDE_DIRS}")
target_link_libraries(cv_calib ${OpenCV_LIBS})
