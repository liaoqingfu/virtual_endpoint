cmake_minimum_required(VERSION 2.8)

project(video_decoder)

set(CMAKE_CXX_FLAGS -std=c++0x)

set(TOOLKITS_DIR $ENV{TOOLKITS})

include_directories( ./
${TOOLKITS_DIR}/include
)

link_directories(${TOOLKITS_DIR}/lib)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

set(SRC_LIST
video_decoder_factory.cpp
video_decoder_v3.cpp 
get_slice_type.cpp
ffmpeg_lib.cpp
video_decoder_ffmpeg.cpp
)


add_library(video_decoder SHARED ${SRC_LIST})

target_link_libraries(video_decoder avcodec swscale avutil avformat)
