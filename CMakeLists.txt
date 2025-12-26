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

# =========================================================
# 2. 核心路径配置 (Fix Path Issues)
# =========================================================
# 定义 OBS 源码位置 (基于 build.yml 的 clone 位置)
set(OBS_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/obs-studio/libobs")

if(EXISTS "${OBS_SRC_DIR}")
    message(STATUS "Found OBS Source at: ${OBS_SRC_DIR}")
else()
    message(WARNING "Could not find OBS source at ${OBS_SRC_DIR}. Please check git clone step.")
endif()

# 使用全局包含目录，确保所有源文件都能看到
include_directories(
    "${OBS_SRC_DIR}"          # 允许 <obs.h> 和 <util/circlebuf.h>
    "${OBS_SRC_DIR}/.."       # 允许 <libobs/obs.h>
    "/opt/homebrew/include"   # simde 和其他 brew 库
    "/usr/local/include"
    "${FFMPEG_INCLUDE_DIRS}"
    "${Python3_INCLUDE_DIRS}"
    "deps/"
)

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
