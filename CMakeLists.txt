cmake_minimum_required(VERSION 3.20)
project(obs-streamlink)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")

if(NOT APPLE)
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

# =========================================================
# [关键修复] 包含目录
# =========================================================
target_include_directories(obs-streamlink PRIVATE
    "${FFMPEG_INCLUDE_DIRS}"
    "${Python3_INCLUDE_DIRS}"
    "deps/"
    "/opt/homebrew/include"
    "/usr/local/include"
)

# [终极路径修复]
if(DEFINED LIBOBS_INCLUDE_DIR)
    target_include_directories(obs-streamlink PRIVATE 
        "${LIBOBS_INCLUDE_DIR}"           # 允许 <obs.h>
        "${LIBOBS_INCLUDE_DIR}/.."        # 允许 <libobs/obs.h>
        "${LIBOBS_INCLUDE_DIR}/util"      # [新增] 允许 "circlebuf.h" 直接引用
    )
endif()

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
    target_link_options(obs-streamlink PRIVATE "-undefined" "dynamic_lookup")
    set_target_properties(obs-streamlink PROPERTIES PREFIX "")
endif()
