#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include "../AbstractMessageHandler.h"
#include "stdint.h"
namespace svlp
{
template <typename handler_t, typename stream_handler_t>
class Message_handler;

template <typename Object1, typename Return1, typename Object2, typename Return2>
class Message_handler<Return1 (Object1::*)(const uint8_t, void* const, const size_t, command_uid_t),
    Return2 (Object2::*)(const uint8_t, void* const, const size_t, bool)> : public AbstractMessageHandler
{
    typedef Return1 (Object1::*my_t)(const uint8_t, void* const, const size_t, command_uid_t);
    typedef Return1 (Object1::*my_t2)(const uint8_t msg_code, void* const param, const size_t param_len, bool isFinal);

public:
    Message_handler(my_t handler, Object1* o1, my_t2 stream_handler = 0, Object2* o2 = 0)
        : handler(handler)
        , stream_handler(stream_handler)
        , o1(o1)
        , o2(o2)
    {
    }

    virtual void handle(const uint8_t msg_code, void* const param, const size_t param_len, command_uid_t cmd_uid)
    {
        if (handler)
        {
            (o1->*handler)(msg_code, param, param_len, cmd_uid);
        }
    }

    virtual void stream_handle(const uint8_t msg_code, void* const param, const size_t param_len, bool isFinal)
    {
        if (stream_handler)
        {
            (o2->*stream_handler)(msg_code, param, param_len, isFinal);
        }
    }

private:
    my_t handler;
    my_t2 stream_handler;

    Object1* o1;
    Object2* o2;
};
}
#endif // MESSAGE_HANDLER_H
