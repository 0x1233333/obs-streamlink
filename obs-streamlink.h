#pragma once

#include <filesystem>

// [修改]：不要写死 "Python38"。
// 为了兼容性，我们暂时将其改为空或者通用的 "python-embed"，
// 但更好的策略是让 OBS 自己的脚本环境去处理，或者我们先改成 OBS 30 常见的 "python310"
// 如果你想更智能，我们可以稍后写一段代码自动检测。
// 目前先改为 OBS 30 Windows 版默认的嵌入式 Python 目录名：
constexpr std::string_view obs_streamlink_python_ver { "python310" }; 

extern std::filesystem::path obs_streamlink_data_path;
