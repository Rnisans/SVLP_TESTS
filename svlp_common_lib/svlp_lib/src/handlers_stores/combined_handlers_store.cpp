#include "headers/handlers_stores/combined_handlers_store.h"
#include <cstring>

svlp::Combined_handlers_store::Combined_handlers_store()
    : commands()
{
}

bool svlp::Combined_handlers_store::is_contain(const svlp::AbstractMessageHandler* const handler, const uint8_t infocode) const
{
    AbstractMessageHandler* const* start = get_start_of_block_by_infocode(infocode);
    AbstractMessageHandler* const* end = get_end_of_block_by_infocode(infocode);

    bool result = false;
    for (AbstractMessageHandler* const* slot = start; slot < end; slot++)
    {
        if (handler == (*slot))
        {
            result = true;
        }
    }
    return result;
}

bool svlp::Combined_handlers_store::remove(svlp::Combined_handlers_store::uid_t uid)
{
    if (uid.is_valid())
    {
        if (commands[uid.value] != 0)
        {
            commands[uid.value] = 0;
            uint8_t infocode = get_infocode_by_uid(uid);
            AbstractMessageHandler** end = get_end_of_block_by_infocode(infocode);
            for (AbstractMessageHandler** current = &commands[uid.value]; current < (end - 1); current++)
            {
                if (current + 1)
                {
                    *current = *(current + 1);
                }
                else
                {
                    break;
                }
            }
            return true;
        }
    }
    return false;
}

void svlp::Combined_handlers_store::clear(uint8_t infocode)
{
    AbstractMessageHandler** start = get_start_of_block_by_infocode(infocode);
    AbstractMessageHandler** end = get_end_of_block_by_infocode(infocode);
    while (start != end)
    {
        *start = 0;
        start++;
    }
}

svlp::AbstractMessageHandler* svlp::Combined_handlers_store::get(const svlp::Combined_handlers_store::uid_t uid) const
{
    if (uid.is_valid())
    {
        return commands[uid.value];
    }
    return 0;
}

svlp::Combined_handlers_store::uid_t svlp::Combined_handlers_store::insert(svlp::AbstractMessageHandler* handler, uint8_t infocode)
{
    svlp::Combined_handlers_store::uid_t result = uid_t();
    if (!is_contain(handler, infocode))
    {
        AbstractMessageHandler** position = 0;
        position = find_empty_place(infocode);
        if (position)
        {
            *position = handler;
            result = uid_t(static_cast<size_t>(position - &commands[0]));
        }
    }
    return result;
}

size_t svlp::Combined_handlers_store::perform_handlers(uint8_t infocode, const uint8_t msg_code, void* const param, const size_t param_len, svlp::command_uid_t cmd_uid)
{
    size_t result = 0;
    AbstractMessageHandler* const* start = get_start_of_block_by_infocode(infocode);
    AbstractMessageHandler* const* end = get_end_of_block_by_infocode(infocode);

    for (AbstractMessageHandler* const* slot = start; slot < end; slot++)
    {
        if (*slot) //check for empty slot
        {
            (*slot)->handle(msg_code, param, param_len, cmd_uid);
            result++;
        }
        else
        {
            break;
        }
    }
    return result;
}

size_t svlp::Combined_handlers_store::perform_stream_handlers(uint8_t infocode, const uint8_t msg_code, void* const param, const size_t param_len, svlp::command_uid_t cmd_uid, bool is_final)
{
    size_t result = 0;
    AbstractMessageHandler* const* start = get_start_of_block_by_infocode(infocode);
    AbstractMessageHandler* const* end = get_end_of_block_by_infocode(infocode);
    for (AbstractMessageHandler* const* slot = start; slot < end; slot++)
    {
        if (*slot) //check for empty slot
        {
            (*slot)->stream_handle(msg_code, param, param_len, is_final, cmd_uid);
            (*slot)->stream_handle(msg_code, param, param_len, is_final);
            result++;
        }
        else
        {
            break;
        }
    }
    return result;
}

bool svlp::Combined_handlers_store::is_insertin_available(uint8_t infocode)
{
    return static_cast<bool>(find_empty_place(infocode)); //если место найдено, значит можно вставлять
}

uint8_t svlp::Combined_handlers_store::get_infocode_by_pointer(svlp::AbstractMessageHandler** position) const
{
    assert(&commands[0] <= position);
    size_t index = static_cast<size_t>(position - &commands[0]);
    uint8_t infocode = 0;
    if (index < 16 * 16)
    {
        infocode = static_cast<uint8_t>(index / 16);
    }
    else if (16 * 16 <= index && index < 16 * 16 + 112 * 4)
    {
        infocode = static_cast<uint8_t>((index - 16 * 16) / 4 + 16);
    }
    else
    {
        infocode = static_cast<uint8_t>((index - 16 * 16 - 112 * 4) + 128);
    }
    return infocode;
}

uint8_t svlp::Combined_handlers_store::get_infocode_by_uid(const svlp::Combined_handlers_store::uid_t uid) const
{
    if (uid.is_valid())
    {
        size_t index = uid.value;
        uint8_t infocode = 0;
        if (index < 16 * 16)
        {
            infocode = static_cast<uint8_t>(index / 16);
        }
        else if (16 * 16 <= index && index < 16 * 16 + 112 * 4)
        {
            infocode = static_cast<uint8_t>((index - 16 * 16) / 4 + 16);
        }
        else
        {
            infocode = static_cast<uint8_t>((index - 16 * 16 - 112 * 4) + 128);
        }
        return infocode;
    }
    return 255;
}

svlp::AbstractMessageHandler** svlp::Combined_handlers_store::find_empty_place(const uint8_t infocode) const
{
    AbstractMessageHandler** start = get_start_of_block_by_infocode(infocode);
    AbstractMessageHandler** end = get_end_of_block_by_infocode(infocode);
    AbstractMessageHandler** result = 0;
    for (AbstractMessageHandler** slot = start; slot < end; slot++)
    {
        if ((*slot) == 0) //empty slot
        {
            result = slot;
            break;
        }
    }
    return result;
}

size_t svlp::Combined_handlers_store::get_block_size_by_infocode(const uint8_t infocode) const
{
    size_t block_size = 0;
    if (infocode < 16) //16x16
    {
        block_size = 16;
    }
    else if (16 <= infocode && infocode < 128)
    {
        block_size = 4;
    }
    else
    {
        block_size = 1;
    }
    return block_size;
}

svlp::AbstractMessageHandler** svlp::Combined_handlers_store::get_start_of_block_by_infocode(uint8_t infocode) const
{
    size_t block_size = get_block_size_by_infocode(infocode);
    return const_cast<AbstractMessageHandler**>(&commands[infocode * block_size]);
}

svlp::AbstractMessageHandler** svlp::Combined_handlers_store::get_end_of_block_by_infocode(const uint8_t infocode) const
{
    size_t block_size = get_block_size_by_infocode(infocode);
    return const_cast<AbstractMessageHandler**>(&commands[(1 + infocode) * block_size]);
}

svlp::Combined_handlers_store::uid_t::uid_t()
    : value(SIZE_MAX)
{
}

svlp::Combined_handlers_store::uid_t::uid_t(size_t value)
    : value(value)
{
}

bool svlp::Combined_handlers_store::uid_t::is_valid() const
{
    return (value != SIZE_MAX);
}
