#ifndef HEARTBEAT_HANDLER_H
#define HEARTBEAT_HANDLER_H

#include "../AbstractMessageHandler.h"
#include "../svlp_writer.h"
namespace svlp
{
class Heartbeat_handler : public svlp::AbstractMessageHandler
{

public:
    explicit Heartbeat_handler(svlp::SVLP_Writer* writer);
    virtual void handle(const uint8_t msg_code, void* const param, const size_t param_len, command_uid_t cmd_uid);

private:
    svlp::SVLP_Writer* writer;
};
}

#endif // HEARTBEAT_HANDLER_H
