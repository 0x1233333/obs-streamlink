#pragma once

#define PY_SSIZE_T_CLEAN
#ifdef _DEBUG
#define STREAMLINK_DEBUG
#endif

#undef _DEBUG
#include <Python.h>

#ifdef STREAMLINK_DEBUG
#define _DEBUG
#endif

#include <string>
#include <functional>
#include <map>
#include <stdexcept> // [新增] 必须包含此头文件

namespace streamlink {
    extern bool loaded;
    extern bool loadingFailed;

    extern PyObject* module;
    namespace methods
    {
        extern PyObject* new_session;
    }

    class ThreadGIL {
        PyGILState_STATE state;
    public:
        ThreadGIL();
        ~ThreadGIL();

        ThreadGIL(ThreadGIL& another) = delete;
        ThreadGIL(ThreadGIL&& another) = default;
        ThreadGIL& operator=(ThreadGIL& another) = delete;
        ThreadGIL& operator=(ThreadGIL&& another) = default;
    };

    std::string PyStringToString(PyObject* pyStr);

    std::string GetExceptionInfo();

    void LogFailure();

    void Initialize();

    class PyObjectHolder
    {
    public:
        PyObject* underlying;
        PyObjectHolder() : PyObjectHolder(nullptr, false) {}
        PyObjectHolder(PyObject* underlying, bool inc = true);
        virtual ~PyObjectHolder();
        PyObjectHolder(PyObjectHolder&& another)noexcept;

        PyObjectHolder& operator=(PyObjectHolder&& another) noexcept;
    };

    class not_loaded : public std::exception {};

    // [修改] 从 std::exception 改为 std::runtime_error 以兼容 macOS/Linux
    class call_failure : public std::runtime_error
    {
    public:
        // std::runtime_error 没有默认构造函数，必须提供消息
        call_failure() : std::runtime_error("Unknown streamlink call failure") {}

        explicit call_failure(char const* message)
            : std::runtime_error(message)
        { }

        // 忽略 int i，因为 runtime_error 不接受它，或者将其格式化进字符串
        call_failure(char const* message, int i)
            : std::runtime_error(message)
        { }

        explicit call_failure(std::exception const& other)
            : std::runtime_error(other.what())
        { }
    };

    class invalid_underlying_object : public std::exception {};
    class stream_ended : public std::exception {};

    class Stream : public PyObjectHolder
    {
    public:
        Stream(PyObject* u);

        Stream(Stream&) = delete;
        Stream(Stream&& another) noexcept;

        int Read(unsigned char* buf, const int len);
        void Close();

    };

    class StreamInfo : public PyObjectHolder
    {
    public:
        std::string name;
        StreamInfo(std::string name, PyObject* u);
        StreamInfo(StreamInfo&& another) noexcept;

        PyObject* Open();
    };

    class Session : public PyObjectHolder {
    private:
        PyObject* set_option;
        PyObjectHolder set_optionGuard;
    public:
        Session();
        ~Session() override;

        std::map<std::string, StreamInfo> GetStreamsFromUrl(std::string const& url);

        void SetOption(std::string const& name, PyObject* value);
        void SetOptionString(std::string const& name, std::string const& value);
        void SetOptionDouble(std::string const& name, double value);
        void SetOptionInt(std::string const& name, long long value);
        void SetOptionBool(std::string const& name, bool value);
    };
}
