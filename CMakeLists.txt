cmake_minimum_required(VERSION 3.20)
project(obs-streamlink)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")

# =========================================================
# 依赖查找部分
# =========================================================

# macOS 下完全跳过 libobs 查找 (防止 FindLibObs.cmake 报错)
if(NOT APPLE)
    find_package(libobs REQUIRED)
endif()

find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(FFmpeg REQUIRED COMPONENTS avcodec avfilter avdevice avutil swscale avformat swresample)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# =========================================================
# 源文件定义
# =========================================================
set(SOURCES
    obs-streamlink.cpp
    streamlink-source.cpp
    media-playback/decode.cpp
    media-playback/media.cpp
    python-streamlink.cpp
)

# Windows 特有文件
if(WIN32)
    list(APPEND SOURCES streamlink-delayload.cpp)
endif()

add_library(obs-streamlink MODULE ${SOURCES})

# =========================================================
# 头文件路径配置
# =========================================================
target_include_directories(obs-streamlink PRIVATE
    "${FFMPEG_INCLUDE_DIRS}"
    "${Python3_INCLUDE_DIRS}"
    "deps/"
    # [修复] 确保能找到 brew 安装的 simde
    "/opt/homebrew/include"
    "/usr/local/include"
)

# [修复] 手动添加我们在 build.yml 里指定的 OBS 头文件路径
if(DEFINED LIBOBS_INCLUDE_DIR)
    target_include_directories(obs-streamlink PRIVATE 
        "${LIBOBS_INCLUDE_DIR}"
        "${LIBOBS_INCLUDE_DIR}/.."
    )
endif()

# =========================================================
# 链接库配置
# =========================================================
target_link_libraries(obs-streamlink PRIVATE
    ${FFMPEG_LIBRARIES}
)

# 仅在非 macOS 平台 (或找到了库文件时) 链接 libobs
if(libobs_FOUND)
    target_link_libraries(obs-streamlink PRIVATE obs::libobs)
endif()

# =========================================================
# 平台特定设置
# =========================================================
if(WIN32)
    target_link_libraries(obs-streamlink PRIVATE Python3::Python)
    target_compile_options(obs-streamlink PUBLIC "/utf-8")
elseif(APPLE)
    # macOS 魔法：允许未定义的符号（运行时由 OBS 主程序提供）
    target_link_options(obs-streamlink PRIVATE "-undefined" "dynamic_lookup")
    
    # 去掉文件名前缀 (生成 obs-streamlink.so 而不是 libobs-streamlink.so)
    set_target_properties(obs-streamlink PROPERTIES PREFIX "")
endif()
