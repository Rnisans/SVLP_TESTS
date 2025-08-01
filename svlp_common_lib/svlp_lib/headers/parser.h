#ifndef ABSTRACT_PARSER
#define ABSTRACT_PARSER

#include "abstract_socket.h"
#include "logger.h"
#include "stdint.h"
#include "tools/noncopyable.h"
#include <stddef.h>
namespace svlp
{
class AbstractMessageHandler;

/* Абстрактный класс парсера командного протокола
 *
 * Класс инкапсулирует работу с сокетом
 *     чтобы реализовать вывод "сырых" данных
 */
//TODO можно сделать перемещаемым. Пока не реализовано, перемещение запрещено
class Parser : public NonCopyable, public NonMovable
{
public:
    /*Конструктор
     * args:
     * socket - сокет для чтения входящих данных
     */
    explicit Parser(ABstractSocket* socket);

    virtual ~Parser();

    /*виртуальный метод, сбрасывает внутреннее состояние парсера в первоначальное.
     * Полезно, например после приёма кривого пакета.
     */
    virtual void reset() = 0;

    /*
     * метод возвращает используемый сокет
     * */
    ABstractSocket* get_socket() const
    {
        return socket;
    }

    void set_socket(ABstractSocket* socket);

    /*
     * Чисто виртуальный метод
     * выполняет чтение и разбор сообщений
     * при необходимости проверяет контрольную суму сообщений
     * после разбора и проверки контрольной суммы передаёт
     *      полученную команду и параметры соответствующему обработчику
     *
     * ret - колличество обработанных команд
     */

    virtual uint32_t parse() = 0;

    /*
    * Устанавливает логер сырых данных
    * args:
    * loger - логер, для логирования "сырых" данных, полученных из сокета
    */
    void set_raw_loger(Loger* const loger)
    {
        raw_loger = loger;
    }

    /*
     * Возвращает последний установленный логер сырых данных
     * в случае, если логер не был установлен возвращает 0
     */
    Loger* get_raw_loger() const
    {
        return this->raw_loger;
    }

protected:
    /* Проверяет наличие доступных для чтения входных данных
     * ret:
     * true - данные доступны для чтения
     * false - нет доступных для чтения данных
    */
    bool is_data_ready() const;

    /*
     * Читает входящие данные и записывает их в буфер, переданный в параметрах
     * при наличии логера, так же выполняет логирование входных данных
     * args:
     * buffer - указатель на буфер памяти,
     *           куда следует сложить входящие данные
     * len - размер буфера памяти
     *
     * ret - реально прочитано байт.
     */

    size_t read_data(void* buffer, const size_t len);

private:
    Loger* raw_loger;
    ABstractSocket* socket;
};
}

#endif // ABSTRACT_PARSER
