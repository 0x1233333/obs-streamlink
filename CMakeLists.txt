cmake_minimum_required(VERSION 3.20)
project(obs-streamlink)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")

# 1. 查找依赖
# macOS 下如果手动提供头文件，这里不再强制 REQUIRED
if(APPLE)
    find_package(libobs QUIET)
else()
    find_package(libobs REQUIRED)
endif()

find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(FFmpeg REQUIRED COMPONENTS avcodec avfilter avdevice avutil swscale avformat swresample)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCES
    obs-streamlink.cpp
    streamlink-source.cpp
    media-playback/decode.cpp
    media-playback/media.cpp
    python-streamlink.cpp
)

if(WIN32)
    list(APPEND SOURCES streamlink-delayload.cpp)
endif()

add_library(obs-streamlink MODULE ${SOURCES})

# 2. 包含目录设置
target_include_directories(obs-streamlink PRIVATE
    "${FFMPEG_INCLUDE_DIRS}"
    "${Python3_INCLUDE_DIRS}"
    "deps/"
)

# [关键修改] 如果未找到 libobs 包（macOS 情况），则使用手动指定的目录
if(NOT libobs_FOUND)
    if(DEFINED LIBOBS_INCLUDE_DIR)
        target_include_directories(obs-streamlink PRIVATE "${LIBOBS_INCLUDE_DIR}")
    else()
        message(WARNING "libobs not found and LIBOBS_INCLUDE_DIR not set! Compilation may fail.")
    endif()
endif()

# 3. 链接设置
target_link_libraries(obs-streamlink PRIVATE
    ${FFMPEG_LIBRARIES}
)

if(libobs_FOUND)
    target_link_libraries(obs-streamlink PRIVATE obs::libobs)
endif()

if(WIN32)
    target_link_libraries(obs-streamlink PRIVATE Python3::Python)
    target_compile_options(obs-streamlink PUBLIC "/utf-8")
elseif(APPLE)
    # macOS 下允许未定义符号 (运行时由 OBS 主程序提供)
    target_link_options(obs-streamlink PRIVATE "-undefined" "dynamic_lookup")
    set_target_properties(obs-streamlink PROPERTIES PREFIX "")
endif()
