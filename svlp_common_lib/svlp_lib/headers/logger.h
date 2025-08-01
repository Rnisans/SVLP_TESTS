#ifndef ABSTRACTLOGER_H
#define ABSTRACTLOGER_H
#include "svlp_header.h"
#include <stddef.h>
#include <stdint.h>

namespace svlp
{
//Абстрактный класс вывода отладочных сообщений
class Loger
{
public:
    //    Чисто виртуальный метод
    //    Обработка сообщения
    //    args:
    //    data - данные, которые необходимо залогировать
    //    len - размер логируемых данных
    //    type - тип данных. В случае логирования структурированных данных, есть возможность
    //        разделять типы данных(например выделять разным цветом, при выводе на консоль)
    virtual void log(const void* data, const size_t len, const uint8_t type = 0) = 0;

    virtual void log(const char* data, const size_t len, const uint8_t = 0) = 0;

    virtual void break_line();

    void log(const svlp_message& msg);

    virtual ~Loger() {}

protected:
};
}

#endif // ABSTRACTLOGER_H
