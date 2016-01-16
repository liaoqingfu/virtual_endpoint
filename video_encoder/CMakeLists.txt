cmake_minimum_required(VERSION 2.8)

project(video_encoder)

set(CMAKE_CXX_FLAGS -std=c++0x)

set(TOOLKITS_DIR $ENV{TOOLKITS})

include_directories( ./
${TOOLKITS_DIR}/include
)

link_directories(${TOOLKITS_DIR}/lib)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

set(SRC_LIST
video_encoder_factory.cpp
x264_lib_v4.cpp
video_encoder_v4.cpp
video_encoder_ffmpeg.cpp
)


add_library(video_encoder SHARED ${SRC_LIST})

target_link_libraries(video_encoder x264)
