#include "headers/parser.h"
#include "assert.h"

//FIX IT
#ifndef assert
#define assert(expression) void(0)
#endif

using namespace svlp;
void Parser::set_socket(ABstractSocket* socket)
{
    //assert(socket);
    this->socket = socket;
}

bool Parser::is_data_ready() const
{
    assert(socket);
    return socket->is_ready_read();
}

size_t Parser::read_data(void* buffer, const size_t len)
{
    assert(socket);
    size_t readed_byte = socket->read(buffer, len);
    if (raw_loger)
    {
        raw_loger->log(buffer, readed_byte);
    }
    return readed_byte;
}

Parser::Parser(ABstractSocket* socket)
    : socket(socket)
{
    //assert(socket);
    raw_loger = 0;
}

Parser::~Parser() {}
