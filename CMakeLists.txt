cmake_minimum_required(VERSION 3.20)
project(obs-streamlink)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")

# =========================================================
# 1. 依赖查找
# =========================================================

# macOS 跳过 libobs 库文件查找
if(NOT APPLE)
    find_package(libobs REQUIRED)
endif()

find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(FFmpeg REQUIRED COMPONENTS avcodec avfilter avdevice avutil swscale avformat swresample)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# =========================================================
# 2. 路径“暴力”修正
# =========================================================
# 如果命令行没传路径，或者路径无效，尝试自动在项目上级目录找
if(NOT EXISTS "${LIBOBS_INCLUDE_DIR}")
    message(STATUS "LIBOBS_INCLUDE_DIR not set or invalid, trying auto-detection...")
    set(LIBOBS_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../obs-studio/libobs")
endif()

message(STATUS "Using LIBOBS path: ${LIBOBS_INCLUDE_DIR}")

# =========================================================
# 3. 源文件
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
# 4. 包含目录 (Include Directories)
# =========================================================
target_include_directories(obs-streamlink PRIVATE
    "${FFMPEG_INCLUDE_DIRS}"
    "${Python3_INCLUDE_DIRS}"
    "deps/"
    "/opt/homebrew/include"
    "/usr/local/include"
    
    # [核心修复] 添加所有可能的 OBS 头文件路径组合
    "${LIBOBS_INCLUDE_DIR}"           # 让 <util/circlebuf.h> 生效
    "${LIBOBS_INCLUDE_DIR}/.."        # 让 <libobs/obs.h> 生效
    "${CMAKE_SOURCE_DIR}/../obs-studio/libobs"  # 硬编码备用路径
)

# =========================================================
# 5. 链接配置
# =========================================================
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
    # macOS 链接参数
    target_link_options(obs-streamlink PRIVATE "-undefined" "dynamic_lookup")
    set_target_properties(obs-streamlink PROPERTIES PREFIX "")
endif()
