cmake_minimum_required(VERSION 3.20)
project(obs-streamlink)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")

# =========================================================
# 1. 依赖查找
# =========================================================
if(NOT APPLE)
    find_package(libobs REQUIRED)
endif()

find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(FFmpeg REQUIRED COMPONENTS avcodec avfilter avdevice avutil swscale avformat swresample)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# =========================================================
# 2. 包含目录 (核心修复)
# =========================================================
# 这里我们显式指定所有可能的路径，确保万无一失
set(OBS_INCLUDE_PATHS
    "${CMAKE_SOURCE_DIR}/obs-studio/libobs"          # 对应 <obs.h>
    "${CMAKE_SOURCE_DIR}/obs-studio/libobs/util"     # 对应 <circlebuf.h>
    "/opt/homebrew/include"
    "/usr/local/include"
    "${FFMPEG_INCLUDE_DIRS}"
    "${Python3_INCLUDE_DIRS}"
    "deps/"
)

# 如果通过命令行传了路径，也加进去
if(DEFINED LIBOBS_INCLUDE_DIR)
    list(APPEND OBS_INCLUDE_PATHS "${LIBOBS_INCLUDE_DIR}")
    list(APPEND OBS_INCLUDE_PATHS "${LIBOBS_INCLUDE_DIR}/util")
endif()

# =========================================================
# 3. 源文件与目标
# =========================================================
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

# 应用包含目录
target_include_directories(obs-streamlink PRIVATE ${OBS_INCLUDE_PATHS})

# =========================================================
# 4. 链接配置
# =========================================================
target_link_libraries(obs-streamlink PRIVATE ${FFMPEG_LIBRARIES})

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
