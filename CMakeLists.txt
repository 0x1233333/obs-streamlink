cmake_minimum_required(VERSION 3.20)
project(obs-streamlink)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")

# 1. 查找依赖
if(NOT APPLE)
    find_package(libobs REQUIRED)
endif()

find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(FFmpeg REQUIRED COMPONENTS avcodec avfilter avdevice avutil swscale avformat swresample)

# 2. 接收并检查绝对路径
if(NOT DEFINED LIBOBS_PATH)
    # 如果本地编译没传参数，尝试自动推断
    set(LIBOBS_PATH "${CMAKE_SOURCE_DIR}/obs-studio/libobs")
endif()

message(STATUS "Build using OBS path: ${LIBOBS_PATH}")

if(NOT EXISTS "${LIBOBS_PATH}/util/circlebuf.h")
    message(WARNING "Critical Header circlebuf.h NOT found in ${LIBOBS_PATH}/util/ ! Build will likely fail.")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# 3. 源文件
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

# 4. 设置包含路径 (使用绝对路径)
target_include_directories(obs-streamlink PRIVATE
    "${LIBOBS_PATH}"          # 这里是 libobs 根目录，使得 <util/circlebuf.h> 生效
    "${LIBOBS_PATH}/.."       # 这里是 obs-studio 根目录，使得 <libobs/obs.h> 生效
    "/opt/homebrew/include"
    "/usr/local/include"
    "${FFMPEG_INCLUDE_DIRS}"
    "${Python3_INCLUDE_DIRS}"
    "deps/"
)

# 5. 链接库
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
