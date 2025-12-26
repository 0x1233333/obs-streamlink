cmake_minimum_required(VERSION 3.20)
project(obs-streamlink)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")

# =========================================================
# [核心修复] 
# 在 macOS 上，我们要完全跳过 find_package(libobs)
# 因为仓库自带的查找脚本太严格，找不到 .dylib 就会报错。
# 我们在 build.yml 里已经手动指定了头文件位置，所以这里直接跳过即可。
# =========================================================
if(NOT APPLE)
    find_package(libobs REQUIRED)
endif()

# 查找其他依赖
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(FFmpeg REQUIRED COMPONENTS avcodec avfilter avdevice avutil swscale avformat swresample)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# 定义源文件
set(SOURCES
    obs-streamlink.cpp
    streamlink-source.cpp
    media-playback/decode.cpp
    media-playback/media.cpp
    python-streamlink.cpp
)

# Windows 特有的延迟加载
if(WIN32)
    list(APPEND SOURCES streamlink-delayload.cpp)
endif()

add_library(obs-streamlink MODULE ${SOURCES})

# 设置头文件路径
target_include_directories(obs-streamlink PRIVATE
    "${FFMPEG_INCLUDE_DIRS}"
    "${Python3_INCLUDE_DIRS}"
    "deps/"
)

# [关键] 手动添加我们在 build.yml 里指定的 OBS 头文件路径
if(DEFINED LIBOBS_INCLUDE_DIR)
    target_include_directories(obs-streamlink PRIVATE "${LIBOBS_INCLUDE_DIR}")
endif()

# 链接库
target_link_libraries(obs-streamlink PRIVATE
    ${FFMPEG_LIBRARIES}
)

# 仅在非 macOS 平台链接 libobs (macOS 使用动态查找)
if(libobs_FOUND)
    target_link_libraries(obs-streamlink PRIVATE obs::libobs)
endif()

# 平台特定设置
if(WIN32)
    target_link_libraries(obs-streamlink PRIVATE Python3::Python)
    target_compile_options(obs-streamlink PUBLIC "/utf-8")
elseif(APPLE)
    # macOS 魔法：允许未定义的符号（运行时由 OBS 主程序提供）
    # 这样我们就不需要链接 libobs 库文件了，彻底避开报错
    target_link_options(obs-streamlink PRIVATE "-undefined" "dynamic_lookup")
    
    # 去掉文件名前缀 (生成 obs-streamlink.so 而不是 libobs-streamlink.so)
    set_target_properties(obs-streamlink PROPERTIES PREFIX "")
endif()
