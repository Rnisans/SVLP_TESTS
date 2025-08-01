#include "headers/handlers/heartbeat_handler.h"

svlp::Heartbeat_handler::Heartbeat_handler(svlp::SVLP_Writer* writer)
    : writer(writer)
{
}

void svlp::Heartbeat_handler::handle(const uint8_t msg_code, void* const param, const size_t param_len, command_uid_t cmd_uid)
{
    (void)cmd_uid;
    switch (msg_code)
    {
    case msg_code_heartbeat_ping:
        svlp::svlp_message msg = writer->create_svlp_message(svlp::infocode_heartbit, msg_code_heartbeat_pong, param, param_len);
        writer->send_message(msg);
        break;
    }
}
