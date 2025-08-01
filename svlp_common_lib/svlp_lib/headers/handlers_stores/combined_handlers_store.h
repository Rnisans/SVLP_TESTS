#ifndef HANDLERS_STORE_H
#define HANDLERS_STORE_H

#include "../AbstractMessageHandler.h"
#include <assert.h>
namespace svlp
{

class Combined_handlers_store
{
    friend class svlplib_test;

public:
    struct uid_t
    {
        friend class Combined_handlers_store;
        uid_t();
        explicit uid_t(size_t value);
        bool is_valid() const;

    private:
        size_t value;
    };

    Combined_handlers_store();
    bool is_contain(AbstractMessageHandler const* const handler, const uint8_t infocode) const;
    bool remove(uid_t uid);
    void clear(uint8_t infocode);
    AbstractMessageHandler* get(const uid_t uid) const;
    uid_t insert(AbstractMessageHandler* handler, uint8_t infocode);
    size_t perform_handlers(uint8_t infocode, const uint8_t msg_code, void* const param, const size_t param_len, command_uid_t cmd_uid);
    size_t perform_stream_handlers(uint8_t infocode, const uint8_t msg_code, void* const param, const size_t param_len, command_uid_t cmd_uid, bool is_final);
    bool is_insertin_available(uint8_t infocode);

private:
    uint8_t get_infocode_by_pointer(AbstractMessageHandler** position) const;
    uint8_t get_infocode_by_uid(const uid_t uid) const;
    AbstractMessageHandler** find_empty_place(const uint8_t infocode) const;
    size_t get_block_size_by_infocode(const uint8_t infocode) const;
    AbstractMessageHandler** get_start_of_block_by_infocode(uint8_t infocode) const;
    AbstractMessageHandler** get_end_of_block_by_infocode(const uint8_t infocode) const;
    AbstractMessageHandler* commands[832];
};
} //namespace svlp

#endif // HANDLERS_STORE_H
