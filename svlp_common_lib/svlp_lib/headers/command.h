#ifndef COMMAND_H
#define COMMAND_H

#include "common_types.h"
#include <cstdint>

namespace svlp
{
class Command
{

public:
    virtual ~Command() {};
    //при успешном выполнении команды
    virtual void onSuccess(void* const param, const size_t param_len, command_uid_t cmd_uid) = 0;
    //при не успешном выполнении команды
    virtual void onFail(void* const param, const size_t param_len, command_uid_t cmd_uid) = 0;
    //при получении сообщения о изменении прогресса выполнения команды
    virtual bool onProgress(void* const param, const size_t param_len, command_uid_t cmd_uid) = 0;
    //при истечении времени ожидания команды
    virtual void onTimeout(command_uid_t cmd_uid) = 0;
    //при получениия ответа о том, что команда не поддерживается
    virtual void onUnsupportCommand(void* const param, const size_t param_len, command_uid_t cmd_uid) = 0;
    //перед удалением обработчика в случае вытеснения его другим обработчиком.
    virtual void onReplace(command_uid_t cmd_uid) = 0;
    // любые другие ответы
    virtual bool onReply(uint8_t msg_code, void* const param, const size_t param_len, command_uid_t cmd_uid) = 0;
};


}

#endif // COMMAND_H
