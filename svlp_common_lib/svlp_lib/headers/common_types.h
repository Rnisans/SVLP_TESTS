#ifndef COMMON_TYPES
#define COMMON_TYPES
#include "stddef.h"
#include "stdint.h"
//typedef uint16_t command_uid_t;

namespace svlp
{

struct command_uid_t
{

    friend class SVLP_Writer;
    template <int buffer_size, class Handlers_store_t>
    friend class Basic_SVLP_Parser;
    template <class Writer_t, class Parser_t>
    friend class Connection;
    friend class svlp_message;

    static command_uid_t min_value()
    {
        return 0;
    }
    static command_uid_t max_value()
    {
        return 0xFFFF;
    }
    bool operator==(const command_uid_t& other)
    {
        return uid == other.uid;
    }

    bool operator<(const command_uid_t& other)
    {
        return uid < other.uid;
    }

    uint32_t to_debug_output()
    {
        return uid;
    }

    command_uid_t()
        : uid(0)
    {
    }

private:
    command_uid_t(uint16_t uid)
        : uid(uid)
    {
    }

    operator uint16_t()
    {
        return uid;
    }

    uint16_t uid;
};

typedef command_uid_t uid_t;

}

//In new code use svlp::command_uid_t
//command_uid_t - deprecated
typedef svlp::command_uid_t command_uid_t;

#endif // COMMON_TYPES
