cmake_minimum_required(VERSION 3.20)
project(obs-streamlink)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")

# 1. 查找依赖
find_package(libobs REQUIRED)
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(FFmpeg REQUIRED COMPONENTS avcodec avfilter avdevice avutil swscale avformat swresample)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# 2. 定义源代码列表
set(SOURCES
    obs-streamlink.cpp
    streamlink-source.cpp
    media-playback/decode.cpp
    media-playback/media.cpp
    python-streamlink.cpp
)

# 3. Windows 特有的延迟加载代码（macOS 不需要）
if(WIN32)
    list(APPEND SOURCES streamlink-delayload.cpp)
endif()

add_library(obs-streamlink MODULE ${SOURCES})

# 4. 包含目录
target_include_directories(obs-streamlink PRIVATE
    "${FFMPEG_INCLUDE_DIRS}"
    "${Python3_INCLUDE_DIRS}"
    "deps/"
)

# 5. 链接库设置
target_link_libraries(obs-streamlink PRIVATE
    obs::libobs
    ${FFMPEG_LIBRARIES}
)

# 6. 平台差异化链接策略
if(WIN32)
    # Windows: 链接具体的 Python 库
    target_link_libraries(obs-streamlink PRIVATE Python3::Python)
    target_compile_options(obs-streamlink PUBLIC "/utf-8")
elseif(APPLE)
    # macOS: 关键设置！
    # 不直接链接 Python 库，而是允许“未定义符号”。
    # 这样插件在运行时会直接使用 OBS 主程序已经加载的 Python 环境，
    # 彻底解决了 macOS 上插件自带 Python 与 OBS 自带 Python 冲突导致的崩溃问题。
    target_link_options(obs-streamlink PRIVATE "-undefined" "dynamic_lookup")
    
    # 移除文件名前缀（让生成的叫 obs-streamlink.so 而不是 libobs-streamlink.so）
    set_target_properties(obs-streamlink PROPERTIES PREFIX "")
endif()
