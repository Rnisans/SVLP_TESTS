#ifndef STCP_SOCKET_ABSTRACT_H
#define STCP_SOCKET_ABSTRACT_H

#include "tools/noncopyable.h"
#include "tools/nonmovable.h"
#include <stdint.h>

namespace svlp
{
enum Socket_type
{
    sTCP,
    sUDP,
    raw_socket,
    video_socket
};

class ABstractSocket : public NonCopyable, public NonMovable
{
public:
    //Конструктор принимает такие параметры:
    //адрес второго уровня точки назначения (ибо у нас точка-точка, адресаации третьего уровня нет)
    //канал
    //тип сокета

    ABstractSocket(uint32_t i_address = 0, uint8_t i_channel = 0, Socket_type i_type = sTCP)
        : address(i_address)
        , channel(i_channel)
        , type(i_type)
    {
    }
    virtual ~ABstractSocket() {}

    //считывает из сокета в буфер buf (это если там что-то есть) и возвращает сколько считал
    virtual uint32_t read(void* const buf, const uint32_t len) = 0;

    //отбросить сколько-то байт из сокета без считывания
    virtual uint32_t skip(uint32_t const len) = 0;

    //возвращает true если есть что прочитать, иначе false
    virtual bool is_ready_read() = 0;

    //принудительно закидывает в сеть содержимое буфера сокета
    virtual void flush() = 0;

    //записывает len байт из *buf
    virtual uint32_t write(const void* buf, const uint32_t len) = 0;

    virtual bool open() = 0;

    virtual bool close() = 0;

protected:
    uint32_t address;
    uint8_t channel;
    Socket_type type;
};
}
#endif // STCP_SOCKET_ABSTRACT_H
