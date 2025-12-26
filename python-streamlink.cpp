#include "python-streamlink.h"

// [删除] #include <Windows.h>  <-- 这一行是导致报错的元凶

#include <vector>
#include <sstream>
#include <iostream>

using namespace streamlink;

// 简单的字符串转换辅助函数
std::string streamlink::PyStringToString(PyObject* pyStr) {
    if (!pyStr) return "";
    Py_ssize_t size;
    const char* ptr = PyUnicode_AsUTF8AndSize(pyStr, &size);
    return ptr ? std::string(ptr, size) : "";
}

// ... (为了确保简单，你可以保留原文件后面的内容，
// 但既然是为了"不出错"，我建议你把这个文件内容全选替换，
// 下面是精简后的核心逻辑，去掉了可能报错的 Windows 垃圾代码)

bool streamlink::loaded = false;
bool streamlink::loadingFailed = false;
PyObject* streamlink::module = nullptr;

namespace streamlink::methods {
    PyObject* new_session = nullptr;
}

void streamlink::Initialize() {
    if (loaded || loadingFailed) return;

    // 初始化 Python (如果在 Mac 上嵌入式运行)
    if (!Py_IsInitialized()) {
        Py_Initialize();
    }

    try {
        // 导入 streamlink 模块
        module = PyImport_ImportModule("streamlink.session");
        if (!module) {
            PyErr_Print();
            throw std::runtime_error("Failed to import streamlink.session");
        }

        PyObject* session_class = PyObject_GetAttrString(module, "StreamlinkSession");
        if (!session_class) {
            throw std::runtime_error("StreamlinkSession class not found");
        }

        methods::new_session = session_class;
        loaded = true;
    } catch (...) {
        loadingFailed = true;
        LogFailure();
        throw;
    }
}

void streamlink::LogFailure() {
    if (PyErr_Occurred()) {
        PyErr_Print();
    }
    std::cerr << "Streamlink failed to initialize." << std::endl;
}

// 实现 Session 构造函数
streamlink::Session::Session() : PyObjectHolder(nullptr) {
    if (!methods::new_session) throw std::runtime_error("Streamlink not initialized");
    
    // 调用 StreamlinkSession()
    PyObject* instance = PyObject_CallObject(methods::new_session, nullptr);
    if (!instance) {
        throw call_failure("Failed to create StreamlinkSession");
    }
    this->underlying = instance;
}

streamlink::Session::~Session() {}

// 实现其他必要的空壳或逻辑 (根据你的原文件逻辑，这里为了防止链接错误，保留基本结构)
// 如果原文件里有复杂的 GetStreamsFromUrl 实现，请保留原文件的剩余部分，
// 只需务必删除顶部的 #include <Windows.h> 即可。
