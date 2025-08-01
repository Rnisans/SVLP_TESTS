#ifndef ABSTRACT_MESSAGE_HANDLER_H
#define ABSTRACT_MESSAGE_HANDLER_H

#include "common_types.h"
#include "stdint.h"
#include "tools/def_deprecation.h"
#include <stddef.h>
namespace svlp
{
//Абстрактный класс обработки входящих команд
class AbstractMessageHandler
{
public:
    AbstractMessageHandler()
    {
    }

    virtual ~AbstractMessageHandler() {}
    /*
 * Чисто виртуальный метод
 * обрабатывает входящие команды
 * args:
 * msg_code - код команды
 * param - параметры команды
 * param_len - размер получаемых параметров
 * cmd_uid - уникальный идентификатор команды.
    */

    virtual void handle(const uint8_t msg_code, void* const param, const size_t param_len, command_uid_t cmd_uid) = 0;

    /*
 * Чисто виртуальный метод
 * обрабатывает входящие команды в потоковом режиме.
 * Используется в случае, если данные в сообщении слишком длинные, чтобы обработать их за один раз.
 * args:
 * msg_code - код команды
 * param - параметры команды
 * param_len - размер получаемых параметров
 * isFinal - Выставляется в true когда в функцию передаётся последняя часть данных.
    */
    DEPRECATED virtual void stream_handle(const uint8_t msg_code, void* const param, const size_t param_len, bool isFinal)
    {
        (void)msg_code;
        (void)param;
        (void)param_len;
        (void)isFinal;
    }
    virtual void stream_handle(const uint8_t msg_code, void* const param, const size_t param_len, bool isFinal, command_uid_t cmd_uid)
    {
        (void)msg_code;
        (void)param;
        (void)param_len;
        (void)isFinal;
        (void)cmd_uid;
    }
};
}

#endif // ABSTRACT_MESSAGE_HANDLER_H
