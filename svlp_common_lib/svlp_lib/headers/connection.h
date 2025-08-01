#ifndef CONNECTION_H
#define CONNECTION_H
#include "../callback/callback.h"
#include "../callback/callback_abstract.h"
#include "commandmonitor.h"
#include "common_types.h"
#include "svlp_parser.h"

#include <utility>

namespace svlp
{

template <class T>
struct item_return
{
    typedef T* pointer_type;
    typedef T type;
};

template <>
struct item_return<void>
{
    typedef void pointer_type;
    typedef void type;
};

class SVLP_Writer;

template <class Writer_t, class Parser_t>
class Connection : private svlp::CommandMonitor
{
public:
    template <class T>
    struct Result
    {
        T data;
        size_t len;
        uint8_t msg_code;
    };

    Connection(Parser_t& parser,
        Writer_t& writer)
        : parser(parser)
        , writer(writer)
        , innrer_callback(&Connection::inner_callback, *this)
    {
        bind(&writer, &parser);
    }

    virtual void handle(const uint8_t msg_code, void* const param, const size_t param_len, command_uid_t cmd_uid)
    {
        if (callback)
            callback->run(cmd_uid, param, param_len, msg_code);
        CommandMonitor::handle(msg_code, param, param_len, cmd_uid);
    }

    template <class T>
    T send_sync();

    template <class T>
    void send_async(); //TODO return

    template <class T>
    std::pair<svlp::msg_code_t, typename item_return<T>::type> send_svlp_message_sync(const uint8_t info_code, const uint8_t msg_code, const void* params, const size_t len)
    {
        svlp::svlp_message msg = writer.create_svlp_message(info_code, msg_code, params, len);
        writer.send_message(msg);

        result.data = 0;
        callback = &innrer_callback;

        Result<T> result = wait_result<T>(msg.header.uid);
        if (result.len >= sizeof(T))
        {
            return std::make_pair(static_cast<svlp::msg_code_t>(msg_code), static_cast<typename item_return<T>::type>(result.data));
        }
        else
        {
            return std::make_pair(static_cast<svlp::msg_code_t>(msg_code), T());
        }
    }

    svlp::msg_code_t send_svlp_message_sync(const uint8_t info_code, const uint8_t msg_code, const void* params, const size_t len)
    {
        this->callback = &innrer_callback;
        svlp::svlp_message msg = writer.create_svlp_message(info_code, msg_code, params, len);
        writer.send_message(msg);
        result.data = 0;

        return static_cast<svlp::msg_code_t>(wait_result<void*>(msg.header.uid).msg_code);
    }

    void send_svlp_message(const uint8_t info_code, const uint8_t msg_code, const void* params, const size_t len, Callback_Abstract* callback)
    {
				svlp::svlp_message msg = writer.create_svlp_message(info_code, msg_code, params, len);
        writer.send_message(msg);
        result.data = 0;
        this->callback = (Callback_Abstract4<command_uid_t&, const void*, size_t, uint8_t>*)callback;
    }

    template <class T>
    Result<T> wait_result(command_uid_t uid)
    {
        pending_uid = uid;
        while (!result.data)
        {
            parser.parse();
        }
        return Result<T> { *static_cast<const T*>(result.data), result.len, result.msg_code };
    }

    void inner_callback(command_uid_t& uid, const void* param, size_t param_len, uint8_t msg_code)
    {
        if (pending_uid == uid)
        {
            result.data = param;
            result.len = param_len;
            result.msg_code = static_cast<svlp::msg_code_t>(msg_code);
        }
    }

private:
    Parser_t& parser;
    Writer_t& writer;

    command_uid_t pending_uid;
    Callback_Abstract4<command_uid_t&, const void*, size_t, uint8_t>* callback;
    Callback<void (Connection::*)(command_uid_t&, const void*, size_t, uint8_t)> innrer_callback;
    struct Result<const void*> result
    {
        0, 0, 0
    };
};
}

#endif // CONNECTION_H
