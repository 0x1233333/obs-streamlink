#pragma once

#include <filesystem>

// 修改为适配新版 OBS 的 Python 目录名
// 如果你的 OBS 插件目录下 Python 文件夹名为 "python310" 或 "python311"，请在此处修改。
// "embed" 是 OBS 官方推荐的嵌入式环境目录名。
constexpr std::string_view obs_streamlink_python_ver { "embed" };

extern std::filesystem::path obs_streamlink_data_path;
