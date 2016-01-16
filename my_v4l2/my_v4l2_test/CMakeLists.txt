cmake_minimum_required(VERSION 2.8)
project(myv4l2test)

INCLUDE(CTest)
ENABLE_TESTING()

add_definitions(-std=c++0x)
if(EXISTS $ENV{TOOLKITS})
message("TOOLKITS is $ENV{TOOLKITS}")
set(TOOLKITS_DIR $ENV{TOOLKITS})
else()
message(FATAL_ERROR "please define TOOLKITS")
endif()
message("----------------------test--------${PROJECT_BINARY_DIR}")
message("=== ${CMAKE_CURRENT_BINARY_DIR}")
SET(CMAKE_MODULE_PATH ${TOOLKITS_DIR}/share/cmake)
FIND_PACKAGE(v4l2 REQUIRED)
if(v4l2_FOUND)
message("toolkits v4l2 is find!")
else()
message(FATAL_ERROR "toolkits v4l2 find Error!")
endif()

find_path(v4l2deviceh v4l2device.h ${PROJECT_SOURCE_DIR}/../)
#find_library(myv4l2lib my_v4l2 ${PROJECT_SOURCE_DIR}/../../lib)

#AUX_SOURCE_DIRECTORY(. SRC_LIST)
set(SRC_LIST test_my_v4l2.cpp )

if(NOT v4l2deviceh)
message(FATAL_ERROR "v4l2device.h not found")
#elseif(NOT myv4l2lib)
#message(FATAL_ERROR "my_v4l2 lib not found")
endif(NOT v4l2deviceh)

include_directories(${v4l2deviceh} ${v4l2_INCLUDE_DIR} ${v4l2_INCLUDE_DIR} ${v4l2_INCLUDE_DIR}/v4l ${PROJECT_SOURCE_DIR})
#link_directories(${PROJECT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

add_executable(myv4l2test ${SRC_LIST})
add_dependencies(myv4l2test my_v4l2)

target_link_libraries(myv4l2test ${v4l2_LIBRARY} my_v4l2)

#install(TARGETS myv4l2test DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

#test

set_property(TARGET myv4l2test PROPERTY LABELS my_v4l2)
ADD_TEST(my_v4l2_init          ${EXECUTABLE_OUTPUT_PATH}/myv4l2test my_v4l2 init)
ADD_TEST(my_v4l2_init_NULL     ${EXECUTABLE_OUTPUT_PATH}/myv4l2test my_v4l2 init_NULL)
ADD_TEST(my_v4l2_release       ${EXECUTABLE_OUTPUT_PATH}/myv4l2test my_v4l2 release)
ADD_TEST(my_v4l2_start_capture ${EXECUTABLE_OUTPUT_PATH}/myv4l2test my_v4l2 start_capture)
ADD_TEST(my_v4l2_stop_capture  ${EXECUTABLE_OUTPUT_PATH}/myv4l2test my_v4l2 stop_capture)
ADD_TEST(my_v4l2_lock_frame    ${EXECUTABLE_OUTPUT_PATH}/myv4l2test my_v4l2 lock_frame)
ADD_TEST(my_v4l2_unlock_frame  ${EXECUTABLE_OUTPUT_PATH}/myv4l2test my_v4l2 unlock_frame)
ADD_TEST(my_v4l2_get_format    ${EXECUTABLE_OUTPUT_PATH}/myv4l2test my_v4l2 get_format)
ADD_TEST(my_v4l2_set_format_yuv420    ${EXECUTABLE_OUTPUT_PATH}/myv4l2test my_v4l2 set_format_yuv420)
ADD_TEST(my_v4l2_set_format_yuv422p   ${EXECUTABLE_OUTPUT_PATH}/myv4l2test my_v4l2 set_format_yuv422p)
ADD_TEST(my_v4l2_get_fps        ${EXECUTABLE_OUTPUT_PATH}/myv4l2test my_v4l2 get_fps)

#ADD_TEST(init_test_video0 ${EXECUTABLE_OUTPUT_PATH}/myv4l2test 3)
#ADD_TEST(start_capture_test ${EXECUTABLE_OUTPUT_PATH}/myv4l2test 4)
#ADD_TEST(v4l2_enum ${EXECUTABLE_OUTPUT_PATH}/myv4l2test 5)
#ADD_TEST(init_test_video1 ${EXECUTABLE_OUTPUT_PATH}/myv4l2test 6)
#ADD_TEST(stop_capture_test ${EXECUTABLE_OUTPUT_PATH}/myv4l2test 7)
#ADD_TEST(get_format_test ${EXECUTABLE_OUTPUT_PATH}/myv4l2test 8)
#ADD_TEST(get_fps_test ${EXECUTABLE_OUTPUT_PATH}/myv4l2test 9)
#ADD_TEST(set_fps_test ${EXECUTABLE_OUTPUT_PATH}/myv4l2test 10)
#ADD_TEST(supported_io_test ${EXECUTABLE_OUTPUT_PATH}/myv4l2test 11)
#ADD_TEST(selected_io_test ${EXECUTABLE_OUTPUT_PATH}/myv4l2test 12)
#ADD_TEST(lock_frame_test ${EXECUTABLE_OUTPUT_PATH}/myv4l2test 13)
#ADD_TEST(unlock_frame_test ${EXECUTABLE_OUTPUT_PATH}/myv4l2test 14)
#ADD_TEST(set_format_false_test ${EXECUTABLE_OUTPUT_PATH}/myv4l2test 15)
#ADD_TEST(get_formats_test ${EXECUTABLE_OUTPUT_PATH}/myv4l2test 16)
#ADD_TEST(get_sizes_test ${EXECUTABLE_OUTPUT_PATH}/myv4l2test 17)
#ADD_TEST(get_intervals_test ${EXECUTABLE_OUTPUT_PATH}/myv4l2test 18)

set_property(TEST 
	my_v4l2_init
	my_v4l2_init_NULL
	my_v4l2_release
	my_v4l2_start_capture
	my_v4l2_stop_capture
	my_v4l2_lock_frame 
	my_v4l2_unlock_frame
	my_v4l2_get_format
	my_v4l2_set_format_yuv420
	my_v4l2_set_format_yuv422p
	my_v4l2_get_fps
PROPERTY LABELS my_v4l2)

#FOREACH(F ${SRC_LIST})
#MESSAGE(${F})
#ENDFOREACH(F)

