#ifndef SIMPLE_HANDLER_STORE_H
#define SIMPLE_HANDLER_STORE_H

#include "../AbstractMessageHandler.h"
namespace svlp
{

class Simple_handler_store
{
public:
    Simple_handler_store();

    struct uid_t
    {
        friend class Simple_handler_store;
        uid_t();
        explicit uid_t(size_t value);
        bool is_valid() const;

    private:
        size_t value;
    };

    bool is_contain(AbstractMessageHandler const* const handler, const uint8_t infocode) const;
    bool remove(uid_t uid);
    void clear(uint8_t infocode);
    AbstractMessageHandler* get(const uid_t uid) const;
    uid_t insert(AbstractMessageHandler* handler, uint8_t infocode);
    size_t perform_handlers(uint8_t infocode, const uint8_t msg_code, void* const param, const size_t param_len, command_uid_t cmd_uid);
    size_t perform_stream_handlers(uint8_t infocode, const uint8_t msg_code, void* const param, const size_t param_len, command_uid_t cmd_uid, bool is_final);
    bool is_insertin_available(uint8_t infocode);

private:
    bool is_slot_empty(uid_t uid) const;
    AbstractMessageHandler* handlers[256];
};

}

#endif // SIMPLE_HANDLER_STORE_H
