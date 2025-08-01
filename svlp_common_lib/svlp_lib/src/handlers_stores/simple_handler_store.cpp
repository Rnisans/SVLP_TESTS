#include "headers/handlers_stores/simple_handler_store.h"
#include <cstring>
#include <limits.h>
#ifndef SIZE_MAX
#ifdef __SIZE_MAX__
#define SIZE_MAX __SIZE_MAX__
#else
#define SIZE_MAX std::numeric_limits<size_t>::max()
#endif
#endif

svlp::Simple_handler_store::Simple_handler_store()
    : handlers()
{
}

bool svlp::Simple_handler_store::is_contain(const svlp::AbstractMessageHandler* const handler, const uint8_t infocode) const
{
    return (handlers[infocode] == handler); //на каждый infocode по одному handler'у
}

bool svlp::Simple_handler_store::remove(svlp::Simple_handler_store::uid_t uid)
{
    bool result = false;
    if (is_slot_empty(uid)) // слот не пуст
    {
        clear(static_cast<uint8_t>(uid.value));
        result = true;
    }
    return result;
}

void svlp::Simple_handler_store::clear(uint8_t infocode)
{
    handlers[infocode] = 0;
}

svlp::AbstractMessageHandler* svlp::Simple_handler_store::get(const svlp::Simple_handler_store::uid_t uid) const
{
    svlp::AbstractMessageHandler* result = 0;
    if (!is_slot_empty(uid))
    {
        result = handlers[uid.value];
    }
    return result;
}

svlp::Simple_handler_store::uid_t svlp::Simple_handler_store::insert(svlp::AbstractMessageHandler* handler, uint8_t infocode)
{
    svlp::Simple_handler_store::uid_t result = uid_t();
    if (!handlers[infocode]) // слот пуст
    {
        handlers[infocode] = handler;
        result = uid_t(infocode);
    }
    return result;
}

size_t svlp::Simple_handler_store::perform_handlers(uint8_t infocode, const uint8_t msg_code, void* const param, const size_t param_len, svlp::command_uid_t cmd_uid)
{
    size_t result = 0;
    if (handlers[infocode])
    {
        handlers[infocode]->handle(msg_code, param, param_len, cmd_uid);
        result = 1;
    }
    return result;
}

size_t svlp::Simple_handler_store::perform_stream_handlers(uint8_t infocode, const uint8_t msg_code, void* const param, const size_t param_len, svlp::command_uid_t cmd_uid, bool is_final)
{
    size_t result = 0;
    if (handlers[infocode])
    {
        handlers[infocode]->stream_handle(msg_code, param, param_len, is_final, cmd_uid);
        //deprecated
        handlers[infocode]->stream_handle(msg_code, param, param_len, is_final);
        result = 1;
    }
    return result;
}

bool svlp::Simple_handler_store::is_insertin_available(uint8_t infocode)
{
    return !static_cast<bool>(handlers[infocode]);
}

bool svlp::Simple_handler_store::is_slot_empty(svlp::Simple_handler_store::uid_t uid) const
{
    return handlers[uid.value];
}

svlp::Simple_handler_store::uid_t::uid_t()
    : value(SIZE_MAX)
{
}

svlp::Simple_handler_store::uid_t::uid_t(size_t value)
    : value(value)
{
}

bool svlp::Simple_handler_store::uid_t::is_valid() const
{
    return (value != SIZE_MAX);
}
