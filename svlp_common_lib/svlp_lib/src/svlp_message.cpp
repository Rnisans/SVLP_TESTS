#include "headers/svlp_message.h"

svlp::svlp_message::svlp_message(const uint8_t infocode, const uint8_t msgcode, void* params)
{
    header.infocode = infocode;
    header.msgcode = msgcode;
    this->params = params;
}
