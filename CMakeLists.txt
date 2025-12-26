cmake_minimum_required(VERSION 3.20)
project(obs-streamlink)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")

# 1. 自动查找已安装的 OBS 库（不再依赖硬编码路径，假设你已经配置好了 libobs 开发环境）
find_package(libobs REQUIRED)

# 2. 移除具体的 Python 3.8 限制，改为查找 Python 3 的开发库
# 注意：OBS 30 通常自带 Python 3.10 或 3.11，这里允许更新的版本
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# 3. 查找 FFmpeg (移除具体的版本号限制，让 CMake 自动找系统里的新版)
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

# 设置包含目录
target_include_directories(obs-streamlink PRIVATE
    "${FFMPEG_INCLUDE_DIRS}"
    "${Python3_INCLUDE_DIRS}"
    "deps/"
)

# 链接库配置
target_link_libraries(obs-streamlink PRIVATE
    obs::libobs           # 使用官方推荐的命名空间链接方式
    ${FFMPEG_LIBRARIES}
    Python3::Python       # 链接找到的 Python
)

# 4. 关键修正：移除所有 /DELAYLOAD 和 /NODEFAULTLIB 的硬编码
# 之前的配置里写死了 python38.dll 和 avcodec-59.dll，这在新版 OBS 上必挂。
# 我们只保留基本的 Windows 链接选项
if(MSVC)
    target_compile_options(obs-streamlink PUBLIC "/utf-8")
    # 如果必须延迟加载（防止启动慢），通常由 OBS 主程序接管，插件尽量少做硬性延迟加载
    # 除非你确定 DLL 名称。为了“不出错”，我们先去掉容易报错的延迟加载。
endif()

# 自动安装脚本 (可选，保留原逻辑但做适配)
if (MSVC)
    add_custom_command(TARGET obs-streamlink POST_BUILD
        COMMAND powershell -Command "Write-Host 'Build Success! Please copy the .dll to your OBS plugins folder manually for safety.'"
    )
endif()
