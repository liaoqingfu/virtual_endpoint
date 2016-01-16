cmake_minimum_required(VERSION 2.8)
project(myvideodevicetest)

INCLUDE(CTest)
ENABLE_TESTING()

add_definitions(-std=c++0x)

if(EXISTS $ENV{TOOLKITS})
message("TOOLKITS is $ENV{TOOLKITS}")
set(TOOLKITS_DIR $ENV{TOOLKITS})
else()
message(FATAL_ERROR "please define TOOLKITS")
endif()

SET(CMAKE_MODULE_PATH ${TOOLKITS_DIR}/share/cmake)
FIND_PACKAGE(v4l2 REQUIRED)
if(v4l2_FOUND)
message("toolkits v4l2 is find!")
else()
message(FATAL_ERROR "toolkits v4l2 find Error!")
endif()

#find_path(myvideodeviceh my_video_device.h ${PROJECT_SOURCE_DIR}/../ )
find_path(v4l2deviceh my_v4l2/v4l2device.h ${PROJECT_SOURCE_DIR}/../..)

#find_library(myv4l2lib my_v4l2 ${SUYUN_SDK_DIR}/lib ${CMAKE_BINARY_DIR}/lib)
#if(NOT myv4l2lib)
#message( "maybe my_v4l2 is not found !!!!!!!!!!!!!!!!")
#endif(NOT myv4l2lib)
#find_library(myvideodevicelib my_video_device ${PROJECT_SOURCE_DIR}/../../lib)

AUX_SOURCE_DIRECTORY(. SRC_LIST)

if(NOT myvideodeviceh AND NOT v4l2deviceh)
message(FATAL_ERROR ".h not found")
#elseif(NOT myv4l2lib AND NOT myvideodevicelib)
#message(FATAL_ERROR "lib not found")
endif(NOT myvideodeviceh AND NOT v4l2deviceh)

include_directories(${PROJECT_SOURCE_DIR}/../ ${v4l2deviceh} ${v4l2_INCLUDE_DIR} ${v4l2_INCLUDE_DIR}/v4l)
link_directories( ${CMAKE_BINARY_DIR}/lib)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
add_executable(myvideodevicetest ${SRC_LIST})
add_dependencies(myvideodevicetest my_video_device my_v4l2)
target_link_libraries(myvideodevicetest my_v4l2 ${v4l2_LIBRARY} my_video_device)

#install(TARGETS myvideodevicetest DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
#test
set_property(TARGET myvideodevicetest PROPERTY LABELS my_video_device)
ADD_TEST(video_device_init_test ${EXECUTABLE_OUTPUT_PATH}/myvideodevicetest 19)
ADD_TEST(run_video_device_test ${EXECUTABLE_OUTPUT_PATH}/myvideodevicetest 20)

set_property(TEST
		video_device_init_test
		run_video_device_test
PROPERTY LABELS my_video_device)
