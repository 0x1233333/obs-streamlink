cmake_minimum_required(VERSION 3.20)
project(obs-streamlink)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")

# 自动查找 OBS 库
find_package(libobs REQUIRED)

# 自动查找 Python 3 (不再限制 3.8，允许 3.10/3.11)
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# 自动查找 FFmpeg (不再限制版本号)
find_package(FFmpeg REQUIRED COMPONENTS avcodec avfilter avdevice avutil swscale avformat swresample)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(obs-streamlink SHARED
    obs-streamlink.cpp
    streamlink-delayload.cpp
    streamlink-source.cpp
    media-playback/decode.cpp
    media-playback/media.cpp
    python-streamlink.cpp
)

target_include_directories(obs-streamlink PRIVATE
    "${FFMPEG_INCLUDE_DIRS}"
    "${Python3_INCLUDE_DIRS}"
    "deps/"
)

target_link_libraries(obs-streamlink PRIVATE
    obs::libobs
    ${FFMPEG_LIBRARIES}
    Python3::Python
)

if(MSVC)
    target_compile_options(obs-streamlink PUBLIC "/utf-8")
endif()
