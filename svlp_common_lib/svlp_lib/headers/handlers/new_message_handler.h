#ifndef NEW_MESSAGE_HANDLER_H
#define NEW_MESSAGE_HANDLER_H

#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include "../AbstractMessageHandler.h"
#include "stdint.h"
#include <functional>
namespace svlp
{

class New_message_handler : public AbstractMessageHandler
{

public:
    New_message_handler(std::function<void(const uint8_t, void* const, const size_t, command_uid_t)> handler,
        std::function<void(const uint8_t msg_code, void* const param, const size_t param_len, bool isFinal)> stream_handler = nullptr)
        : handler(handler)
        , stream_handler(stream_handler)
    {
    }

    virtual void handle(const uint8_t msg_code, void* const param, const size_t param_len, command_uid_t cmd_uid)
    {
        if (handler)
        {
            handler(msg_code, param, param_len, cmd_uid);
        }
    }

    virtual void stream_handle(const uint8_t msg_code, void* const param, const size_t param_len, bool isFinal)
    {
        if (stream_handler)
        {
            stream_handler(msg_code, param, param_len, isFinal);
        }
    }

private:
    std::function<void(const uint8_t, void* const, const size_t, command_uid_t)> handler;
    std::function<void(const uint8_t msg_code, void* const param, const size_t param_len, bool isFinal)> stream_handler;
};
}
#endif // MESSAGE_HANDLER_H

#endif // NEW_MESSAGE_HANDLER_H
