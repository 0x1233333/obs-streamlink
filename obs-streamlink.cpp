#include "obs-streamlink.h"
#include <obs-module.h>
#include <filesystem>

// 移除对 "obs-source-old.h" 的引用，防止结构体冲突
// #include "obs-source-old.h" 

#include "python-streamlink.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-streamlink", "en-US")

MODULE_EXPORT const char *obs_module_description(void)
{
	return "Streamlink Source";
}

// 使用标准的 obs_source_info，不再用 old 版本
extern "C" obs_source_info streamlink_source_info;
std::filesystem::path obs_streamlink_data_path;

bool obs_module_load(void)
{
	std::string data_path = obs_get_module_data_path(obs_current_module());
	obs_streamlink_data_path = data_path;
	
    // 检查 Python 环境是否存在
	if (!std::filesystem::exists(obs_streamlink_data_path / obs_streamlink_python_ver))
	{
		blog(LOG_ERROR, "[Streamlink Source] Failed to initialize: Python environment not found at %s/%s", data_path.c_str(), obs_streamlink_python_ver.data());
		return false;
	}

	streamlink::Initialize();
    
    // 使用标准方式注册源，自动处理结构体大小
	obs_register_source(&streamlink_source_info);
    
	return true;
}
