#include "obs-streamlink.h"
#include <obs-module.h>
#include <filesystem>
#include "python-streamlink.h"

// 移除旧头文件引用，防止冲突
// #include "obs-source-old.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-streamlink", "en-US")

MODULE_EXPORT const char *obs_module_description(void)
{
	return "Streamlink Source";
}

extern "C" obs_source_info streamlink_source_info;
std::filesystem::path obs_streamlink_data_path;

bool obs_module_load(void)
{
	std::string data_path = obs_get_module_data_path(obs_current_module());
	obs_streamlink_data_path = data_path;

    // [跨平台兼容修改]
    // Windows: 通常需要检查 Python 是否存在于插件数据目录
    // macOS: Python 环境由 OBS 主程序提供，插件目录里通常没有 Python
    // 策略：只在 Windows 下进行严格检查，其他平台(macOS/Linux) 尝试直接加载。
#ifdef _WIN32
	if (!std::filesystem::exists(obs_streamlink_data_path / obs_streamlink_python_ver))
	{
		blog(LOG_ERROR, "[Streamlink Source] Error: Python environment not found at %s/%s", data_path.c_str(), obs_streamlink_python_ver.data());
		return false;
	}
#else
    // macOS/Linux: 仅记录日志，不强制返回 false，相信系统或 OBS 环境
    blog(LOG_INFO, "[Streamlink Source] Non-Windows system detected. Assuming Python is provided by host OBS.");
#endif

	try {
		streamlink::Initialize();
	} catch (...) {
		blog(LOG_ERROR, "[Streamlink Source] Critical Exception during Streamlink initialization!");
		return false;
	}

    // 使用标准注册函数
	obs_register_source(&streamlink_source_info);
	return true;
}
