#include "headers/commandmonitor.h"

void svlp::CommandMonitor::tick(uint64_t ticks)
{
    counter += ticks;
    for (size_t i = 0; i < max_command_counter; i++)
    {
        if (!cmds[i].empty && cmds[i].fire_time <= counter)
        {
            cmds[i].cmd->onTimeout(cmds[i].uid);
            cmds[i].empty = true;
        }
    }
}

bool svlp::CommandMonitor::addCommand(svlp::Command* cmd, command_uid_t uid, uint8_t msg_code, uint32_t timeout)
{
    bool success = false;
    command_uid_t min_uid = command_uid_t::max_value();
    size_t min_index = 0;
    for (size_t i = 0; i < max_command_counter; i++)
    {
        if (cmds[i].uid < min_uid)
        {
            min_uid = cmds[i].uid;
            min_index = i;
        }
        if (cmds[i].empty)
        {
            cmds[i].empty = false;
            cmds[i].cmd = cmd;
            cmds[i].uid = uid;
            cmds[i].fire_time = timeout + counter;
            cmds[i].timeout = timeout;
            cmds[i].msg_code = msg_code;
            success = true;
            break;
        }
    }

    //если не нашлось места для вставки новой команды. Заменяем команду с наименьшим uid
    if (!success)
    {
        cmds[min_index].cmd->onReplace(cmds[min_index].uid);
        cmds[min_index].empty = false;
        cmds[min_index].cmd = cmd;
        cmds[min_index].uid = uid;
        cmds[min_index].fire_time = timeout + counter;
        cmds[min_index].timeout = timeout;
        success = true;
    }
    return success;
}

bool svlp::CommandMonitor::deleteCommand_by_uid(command_uid_t uid)
{
    bool success = false;
    for (size_t i = 0; i < max_command_counter; i++)
    {
        if ((cmds[i].uid == uid) && !cmds[i].empty)
        {
            cmds[i].empty = true;
            success = true;
        }
    }
    return success;
}

bool svlp::CommandMonitor::deleteCommand_by_msg_code(uint8_t msg_code)
{
    bool success = false;
    for (size_t i = 0; i < max_command_counter; i++)
    {
        if ((cmds[i].msg_code == msg_code) && !cmds[i].empty)
        {
            cmds[i].empty = true;
            success = true;
        }
    }
    return success;
}

bool svlp::CommandMonitor::set_default_command(svlp::Command* cmd)
{
    bool ret = default_command != NULL;
    this->default_command = cmd;
    return ret;
}

svlp::Command* svlp::CommandMonitor::get_default_command() const
{
    return this->default_command;
}

void svlp::CommandMonitor::handle(const uint8_t msg_code, void* const param, const size_t param_len, command_uid_t cmd_uid)
{
    commad_s* entry = 0;
    Command* cmd = 0;
    for (size_t i = 0; i < max_command_counter; i++)
    {
        if (cmds[i].uid == cmd_uid && !cmds[i].empty)
        {
            entry = &cmds[i];
            cmd = cmds[i].cmd;
        }
    }

    switch (msg_code)
    {
    case msg_code_result_success:
        if (cmd)
        {
            cmd->onSuccess(param, param_len, cmd_uid);
            deleteCommand_by_uid(cmd_uid);
        }
        else if (default_command)
        {
            default_command->onSuccess(param, param_len, cmd_uid);
        }
        break;
    case msg_code_result_fail:
        if (cmd)
        {
            cmd->onFail(param, param_len, cmd_uid);
            deleteCommand_by_uid(cmd_uid);
        }
        else if (default_command)
        {
            default_command->onFail(param, param_len, cmd_uid);
        }
        break;
    case msg_code_result_progress:
    {
        if (cmd)
        {
            bool isTerminal = cmd->onProgress(param, param_len, cmd_uid);
            if (isTerminal)
            {
                deleteCommand_by_uid(cmd_uid);
            }
            else
            {
                entry->fire_time = entry->timeout + counter;
            }
        }
        else if (default_command)
        {
            default_command->onProgress(param, param_len, cmd_uid);
        }
        break;
    }
    case msg_code_result_unsupported:
        if (cmd)
        {
            cmd->onUnsupportCommand(param, param_len, cmd_uid);
            deleteCommand_by_uid(cmd_uid);
        }
        else if (default_command)
        {
            default_command->onUnsupportCommand(param, param_len, cmd_uid);
        }
        break;
    default:
        if (cmd)
        {
            bool isTerminal = cmd->onReply(msg_code, param, param_len, cmd_uid);
            if (isTerminal)
            {
                deleteCommand_by_uid(cmd_uid);
            }
            else
            {
                entry->fire_time = entry->timeout + counter;
            }
        }
        else if (default_command)
        {
            default_command->onReply(msg_code, param, param_len, cmd_uid);
        }
        break;
    }
}

void svlp::CommandMonitor::stream_handle(const uint8_t msg_code, void* const param, const size_t param_len, bool isFinal)
{
    (void)msg_code;
    (void)param;
    (void)param_len;
    (void)isFinal;
}
