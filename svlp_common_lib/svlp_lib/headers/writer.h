#ifndef WRITER
#define WRITER

#include "abstract_socket.h"
#include "logger.h"
#include "stdint.h"
#include "tools/noncopyable.h"
#include <stddef.h>

namespace svlp
{
/*
 * Абстрактный класс писателя командных протколов
 * Из входных данных, формирует сообщение и отправляет его
 * Класс инкапсулирует работу с сокетом
 *     чтобы реализовать вывод "сырых" данных
 *
 */
class Writer : public NonCopyable
{
public:
    /* Конструктор
     * args:
     * socket - сокет для чтения входящих данных
     */
    Writer(ABstractSocket& socket)
        : loger(0)
        , socket(socket)

    {
    }

    /*
     * Устанавливает логер сырых данных
     * args:
     * loger - логер, для логирования "сырых" данных,
     *           записанных в сокет
     */
    void set_loger(Loger* loger)
    {
        this->loger = loger;
    }

    Loger* getLoger() const
    {
        return loger;
    }

    ABstractSocket& getSocket() const
    {
        return socket;
    }

    virtual ~Writer() {}

protected:
    /* Метод отправки данных в сокет
     * При наличии логера, логирует их
     * ret:
     * колличество отправленны байт
     *
     */
    uint32_t write_data(const void* buffer, const size_t len)
    {
        uint32_t byte_sended = socket.write(buffer, len);
        if (loger)
        {
            loger->log(buffer, byte_sended);
        }
        return byte_sended;
    }

private:
    Loger* loger;
    ABstractSocket& socket;
};
}
#endif // WRITER
