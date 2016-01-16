cmake_minimum_required(VERSION 2.8)
project(video_capture_test)

#message("@@@@@@@@@@@@@@@${PROJECT_SOURCE_DIR}/../../lib/video/")
#SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../lib/video/)
add_definitions(-std=c++0x) 

IF(EXISTS $ENV{TOOLKITS} )
MESSAGE("1. TOOLKITS is $ENV{TOOLKITS}...")
ELSE()
MESSAGE(FATAL_ERROR "-1. Please Define TOOLKITS!")
ENDIF()

SET(TOOLKITS_DIR $ENV{TOOLKITS})
MESSAGE("2. TOOLKITS_DIR is ${TOOLKITS_DIR}...")


IF(EXISTS $ENV{SUYUN_SDK} )
MESSAGE(" 1. suyun SDK is $ENV{SUYUN_SDK}...")
ELSE()
MESSAGE(FATAL_ERROR "-1. Please Define SUYUN_SDK!")
ENDIF()

SET(SUYUN_SDK_DIR $ENV{SUYUN_SDK})

include_directories(${v4l2_INCLUDE_DIR} ${v4l2_INCLUDE_DIR}/v4l)
include_directories(${TOOLKITS_DIR}/include)

link_directories(${CMAKE_BINRARY_PATH}/lib )
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

set(SRC_LIST test_video_capture.cpp)
add_executable(main_capture ${SRC_LIST})

target_link_libraries(main_capture video_capture my_v4l2 ${v4l2_LIBRARY} dl)

set_property(TARGET main_capture PROPERTY LABELS video_capture)
add_test(capture_init  ${EXECUTABLE_OUTPUT_PATH}/main_capture capture_init)
add_test(capture_release   ${EXECUTABLE_OUTPUT_PATH}/main_capture capture_release)
add_test(capture_set_video_param    ${EXECUTABLE_OUTPUT_PATH}/main_capture capture_set_video_param)
add_test(capture_capture ${EXECUTABLE_OUTPUT_PATH}/main_capture capture_capture)
add_test(capture_operator ${EXECUTABLE_OUTPUT_PATH}/main_capture capture_operator)

set_property(TEST 
		capture_init  
		capture_release   
		capture_set_video_param    
		capture_capture
        capture_operator
 PROPERTY LABELS video_capture)

