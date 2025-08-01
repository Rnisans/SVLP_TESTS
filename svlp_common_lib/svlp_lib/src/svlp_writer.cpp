#include "headers/svlp_writer.h"
#include "../crc/crc32_apple.h"
#include "headers/commandmonitor.h"
#if (defined NIOS2 || defined __arm__)
#include <alloca.h>
#elif defined __ICCARM__
#include <stdlib.h>
#else
#include <malloc.h>
#endif

#include <cstring>
using namespace svlp;
SVLP_Writer::SVLP_Writer(ABstractSocket& socket, bool crc_enable)
    : Writer(socket)
    , is_crc_enable(crc_enable)
    , uid(0)
    , shot_message_size(0)
    , cmdMonitor(0)
{
}

bool SVLP_Writer::get_is_crc_enable() const
{
    return is_crc_enable;
}

void SVLP_Writer::set_is_crc_enable(bool value)
{
    is_crc_enable = value;
}

svlp_message SVLP_Writer::create_svlp_message(const uint8_t info_code,
    const uint8_t msg_code,
    const void* params,
    const size_t len)
{
    svlp_message msg;
    msg.header.reserve = 0;
    msg.header.msgcode = msg_code;
    msg.header.infocode = info_code;
    msg.header.params_length = len;
    msg.header.version = 1;
    msg.header.uid = uid++;
    msg.header.flags = is_crc_enable ? is_crc_mask : 0;
    msg.params = const_cast<void*>(params);

    msg.crc = 0;
    return msg;
}

svlp_message SVLP_Writer::create_svlp_reply(command_uid_t cmd_uid, const uint8_t msg_code, const void* params, const size_t len)
{
    svlp_message msg = create_svlp_message(infocode_cmd_result, msg_code, params, len);
    msg.header.uid = cmd_uid;
    return msg;
}

uint32_t SVLP_Writer::calc_crc(const void* buf, const size_t size, uint32_t crc) const
{
    return ComputeCRC32(crc, static_cast<const uint8_t*>(buf), size);
}

uint32_t SVLP_Writer::forced_write(const void* data, uint32_t len)
{
    uint32_t sended_byte = 0;
    uint32_t count = 1000;
    while (count-- && sended_byte < len)
    {
        sended_byte += write_data(&(static_cast<const char*>(data)[sended_byte]), len - sended_byte);
    }
    return sended_byte;
}

uint32_t SVLP_Writer::send_shot_message(const svlp_message& msg)
{

    const uint32_t msg_size = 4 + sizeof(msg.header) + msg.header.params_length + (is_crc_enable ? 4 : 0);
		
#if defined __ICCARM__
		uint8_t* data = static_cast<uint8_t*>(malloc(msg_size));
#else
		uint8_t* data = static_cast<uint8_t*>(alloca(msg_size));
#endif

    memcpy(&data[0], "\xAA\xAA\xAA\xAA", 4); // Кладём в буфер разделитель
    memcpy(&(data[4]), &(msg.header), sizeof(msg.header)); // Следом за ним заголовок
    memcpy(&(data[4 + sizeof(msg.header)]), msg.params, msg.header.params_length); // И данные

    if (is_crc_enable)
    {
        uint32_t crc = calc_crc(&msg.header, sizeof(svlp_message_header), 0);
        crc = calc_crc(msg.params, msg.header.params_length, crc);
        memcpy(&data[4 + sizeof(msg.header) + msg.header.params_length], &crc, 4); // При необходимости контрольную сумму
    }
	uint32_t sended_byte = forced_write(data, msg_size);
		
#if defined __ICCARM__
		free(data);
#endif
    return sended_byte;
}

uint32_t SVLP_Writer::get_shot_message_size() const
{
    return shot_message_size;
}

void SVLP_Writer::set_shot_message_size(const uint32_t& value)
{
    shot_message_size = value;
}

CommandMonitor* SVLP_Writer::getCmdMonitor() const
{
    return cmdMonitor;
}

void SVLP_Writer::setCmdMonitor(CommandMonitor* value)
{
    cmdMonitor = value;
}

uint32_t SVLP_Writer::send_long_message(const svlp_message& msg)
{
    uint32_t sended_byte = 0;
    struct
    {
        uint8_t spliter[4];
        svlp_message_header header;

    } header = { { 0xAA, 0xAA, 0xAA, 0xAA }, { 0, 0, 0, 0, 0, 0, 0 } };
    header.header = msg.header;

    sended_byte += forced_write(&header, sizeof(header));
    sended_byte += forced_write(msg.params, msg.header.params_length);

    if (is_crc_enable)
    {
        uint32_t crc = calc_crc(&msg.header, sizeof(svlp_message_header), 0);
        crc = calc_crc(msg.params, msg.header.params_length, crc);
        sended_byte += forced_write(&crc, 4);
    }
    return sended_byte;
}

uint32_t SVLP_Writer::send_message(const svlp_message& msg, Command* cmd, uint32_t timeout)
{
    uint32_t sended_byte = 0;
    if (msg.header.params_length <= shot_message_size)
    {
        sended_byte = send_shot_message(msg);
        getSocket().flush();
    }
    else
    {
        sended_byte = send_long_message(msg);
        getSocket().flush();
    }
    if (cmdMonitor && cmd)
    {
        cmdMonitor->addCommand(cmd, msg.header.uid, msg.header.msgcode, timeout);
    }
    return sended_byte;
}
