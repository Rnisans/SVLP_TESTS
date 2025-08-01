#ifndef RESPONC_HANDLER_H
#define RESPONC_HANDLER_H

#include "headers/command.h"
#include <assert.h>
#include <functional>
#include <vector>

enum Resut
{
    success,
    fail,
    progress,
    unsupported,
    unknown,
    replace,
    timeout
};

template <class... Args>
struct Responce_handler : public svlp::Command
{

public:
    Responce_handler(std::function<void(Resut, Args...)> f)
        : f(f)
        , f_no_param(nullptr)
    {
    }

    Responce_handler()
    {
        Responce_handler(nullptr);
    }
    // Command interface

    template <typename T>
    struct type
    {
    };

    //Структура для получения количества аргументов класса Responce_handler
    template <typename H>
    struct size_
    {
        static const size_t val = sizeof...(Args);
    };

    // Эта функция используется если в Responce_handler есть один или несколько аргументов
    template <typename K>
    typename std::enable_if<size_<K>::val, void>::type
    success_t(void* const param, const size_t param_len, command_uid_t cmd_uid)
    {
        run_success(type<Args...>(), param, param_len, cmd_uid);
    }

    // Эта функция используется если в Responce_handler отсутствуют аргументы
    template <typename K>
    typename std::enable_if<!size_<K>::val, void>::type
    success_t(void* const param, const size_t param_len, command_uid_t cmd_uid)
    {
        run_success(param, param_len, cmd_uid);
    }

    // Эта функция используется если в Responce_handler есть один или несколько аргументов
    template <typename K>
    typename std::enable_if<size_<K>::val, void>::type
    fail_t(void* const param, const size_t param_len, command_uid_t cmd_uid)
    {
        run_fail(type<Args...>(), param, param_len, cmd_uid);
    }

    // Эта функция используется если в Responce_handler отсутствуют аргументы
    template <typename K>
    typename std::enable_if<!size_<K>::val, void>::type
    fail_t(void* const param, const size_t param_len, command_uid_t cmd_uid)
    {
        if (f_no_param != nullptr)
        {
            f_no_param(fail, cmd_uid);
        }
    }

    void set_f_param(std::function<void(Resut, command_uid_t)> f_param)
    {
        f_no_param = f_param;
    }

    void onSuccess(void* const param, const size_t param_len, command_uid_t cmd_uid)
    {
        // if constexpr был заменён на enable_if, так как проект надо было собрать под C++11, а constexpr появился только в C++17
        success_t<bool>(param, param_len, cmd_uid);
    }

    void run_success(void* const param, const size_t param_len, command_uid_t cmd_uid)
    {
        if (f != nullptr)
        {
            f(success);
        }
    }

    template <typename T>
    void run_success(type<T>, void* const param, const size_t param_len, command_uid_t cmd_uid)
    {
        if (f != nullptr)
        {
            f(success, *static_cast<T*>(param));
        }
    }

    template <typename T>
    void run_success(std::vector<T>, void* param, const size_t param_len, command_uid_t cmd_uid)
    {
        std::vector<T> vector;
        T* i_data = static_cast<T*>(param);

        for (int i = 0; i < param_len / sizeof(T); i++)
        {
            vector.push_back(i_data[i]);
        }
        if (f != nullptr)
        {
            f(success, vector);
        }
    }

    void onFail(void* const param, const size_t param_len, command_uid_t cmd_uid)
    {
        fail_t<bool>(param, param_len, cmd_uid);
    }

    template <typename T>
    void run_fail(type<T>, void* const param, const size_t param_len, command_uid_t cmd_uid)
    {
        if (f != nullptr)
        {
            f(fail, *static_cast<T*>(param));
        }
    }

    bool onProgress(void* const param, const size_t param_len, command_uid_t cmd_uid)
    {
        return false;
    }

    /*template <class T>
    void run_progress(void* const param, const size_t param_len, command_uid_t cmd_uid)
    {
        f(progress, *static_cast<T*>(param));
    }*/

    void onTimeout(command_uid_t cmd_uid)
    {
        if (f_no_param != nullptr)
        {
            f_no_param(timeout, cmd_uid);
        }
    }

    void onUnsupportCommand(void* const param, const size_t param_len, command_uid_t cmd_uid)
    {
        if (f_no_param != nullptr)
        {
            f_no_param(unsupported, cmd_uid);
        }
    }

    void onReplace(command_uid_t cmd_uid)
    {
        if (f_no_param != nullptr)
        {
            f_no_param(replace, cmd_uid);
        }
    }

    bool onReply(uint8_t msg_code, void* const param, const size_t param_len, command_uid_t cmd_uid)
    {
        return false;
    }

    /*template <class T>
    void run_reply(void* const param, const size_t param_len, command_uid_t cmd_uid)
    {
        f(unsupported, *static_cast<T*>(param));
    }*/

private:
    std::function<void(Resut, Args...)> f;
    std::function<void(Resut, command_uid_t)> f_no_param;
};

#endif // RESPONC_HANDLER_H
