#ifndef SVLP_MESSAGE_H
#define SVLP_MESSAGE_H

#include "common_types.h"
#include "message.h"
#include "stdint.h"

namespace svlp
{

/*
 * Маска, для проверки состояния флага наличия контрольной суммы
 *      в SVLP пакете.
 */
static const uint8_t is_crc_mask = 0x01;
static const uint8_t is_reply_request_mask = 0x02;

/*
 * Структура заголовка SVLP пакета.
 */
#pragma pack(push, 1)
struct svlp_message_header
{
    uint8_t version; //версия
    uint8_t flags; //флаги
    uint8_t infocode; //тип сообщения
    uint8_t msgcode; //код сообщения
    uint32_t params_length; //длина массива параметров
    uint16_t uid; //идентификатор команды
    uint16_t reserve; //Резерв
};

struct svlp_message : public message
{
    svlp_message(const uint8_t infocode, const uint8_t msgcode, void* data);
    svlp_message() {}

    void set_uid(command_uid_t uid)
    {
    	header.uid = uid;
    }

private:
    friend class SVLP_Writer;
    template <int buffer_size, class Handlers_store_t>
    friend class Basic_SVLP_Parser;
    friend class Loger;
    template <class Writer_t, class Parser_t>
    friend class Connection;

    svlp_message_header header;
    void* params;
    uint32_t crc;
};
#pragma pack(pop)

enum info_code_t
{
    infocode_heartbeat = 0, //пустое сообщение, heartbeat
    infocode_heartbit = infocode_heartbeat, //deprecated. use infocode_heartbeat instead.
    infocode_cmd, //команда
    infocode_cmd_result, //результат выполнения команды
    infocode_event, //обытие
    infocode_error, //ошибка
    infocode_telemetry //телеметрия
};

enum msg_code_t
{
    msg_code_heartbeat_ping = 1,
    msg_code_heartbeat_pong = 2,

    msg_code_cmd_reboot = 1,
    msg_code_cmd_get_info = 2,
    msg_code_cmd_get_status = 3,

    msg_code_reply_success = 1,
    msg_code_reply_fail = 2,
    msg_code_reply_progress = 3,
    msg_code_reply_unsupported = 10,

    msg_code_event_turn_on = 1,
    msg_code_event_reboot = 2,
    msg_code_event_status_changed = 3,

    msg_code_error_unsupported_protocol_version = 1,
    msg_code_error_unknown_parameter_format = 2
};

}

#endif // SVLP_MESSAGE_H
