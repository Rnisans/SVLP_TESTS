#include "headers/logger.h"
using namespace svlp;
void Loger::break_line()
{

    log("\0", 0, 0);
}

void Loger::log(const svlp_message& msg)
{
    log(&msg.header, sizeof(msg.header), 1);
    log(msg.params, msg.header.params_length, 2);
    if (msg.header.flags & is_crc_mask)
    {
        log(&msg.crc, 4, 3);
    }
    break_line();
}
