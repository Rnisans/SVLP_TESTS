#ifndef COMMANDMONITOR_H
#define COMMANDMONITOR_H

#include "AbstractMessageHandler.h"
#include "command.h"
#include "svlp_parser.h"
#include "svlp_writer.h"

namespace svlp
{

class CommandMonitor : public AbstractMessageHandler
{
    enum msg_code_t
    {
        msg_code_result_success = 1,
        msg_code_result_fail = 2,
        msg_code_result_progress = 3,
        msg_code_result_unsupported = 10
    };
    static const int max_command_counter = 16;

public:
    CommandMonitor()
        : counter(0)
        , default_command(0)
    {
    }

    void tick(uint64_t ticks = 1);
    template <class Writer_t, class Parser_t>
    bool bind(Writer_t* writer, Parser_t* parser)
    {
        bool success = false;
        if (parser && writer)
        {
            parser->setCmdMonitor(this);
            writer->setCmdMonitor(this);
            success = true;
        }
        else
        {
            success = false;
        }
        return success;
    }

    bool addCommand(Command* cmd, command_uid_t uid, uint8_t msg_code, uint32_t timeout);
    bool deleteCommand_by_uid(command_uid_t uid);
    bool deleteCommand_by_msg_code(uint8_t msg_code);

    bool set_default_command(Command* cmd);
    Command* get_default_command() const;
		
		using AbstractMessageHandler::stream_handle;

    virtual void handle(const uint8_t msg_code, void* const param, const size_t param_len, command_uid_t cmd_uid);
    virtual void stream_handle(const uint8_t msg_code, void* const param, const size_t param_len, bool isFinal);

private:
    struct commad_s
    {
        commad_s()
            : empty(true)
        {
        }
        uint64_t fire_time;
        Command* cmd;
        uint32_t timeout;
        command_uid_t uid;
        bool empty;
        uint8_t msg_code;
    };
    uint64_t counter;

    Command* default_command;

    commad_s cmds[max_command_counter];
};
}
#endif // COMMANDMONITOR_H
