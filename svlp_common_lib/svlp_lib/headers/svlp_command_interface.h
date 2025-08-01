#ifndef SVLP_COMMAND_INTERFACE_H
#define SVLP_COMMAND_INTERFACE_H

#include "headers/svlp_header.h"

class SVLP_Command_interface
{
public:
    virtual void reboot(svlp::command_uid_t cmd_uid) = 0;
    virtual void get_info(svlp::command_uid_t cmd_uid) = 0;
    virtual void get_status(svlp::command_uid_t cmd_uid) = 0;

    virtual ~SVLP_Command_interface() {}
};
#endif // SVLP_COMMAND_INTERFACE_H
