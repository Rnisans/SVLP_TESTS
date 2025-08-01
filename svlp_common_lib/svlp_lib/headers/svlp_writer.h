#ifndef SVLP_WRITER_H
#define SVLP_WRITER_H
#include "common_types.h"
#include "stdint.h"
#include "svlp_header.h"
#include "tools/static_assert.h"
//#include "tools/type_traits.h"
#include "writer.h"
#include <stddef.h>

namespace svlp
{
class CommandMonitor;
class Command;

/*
 * Класс писателя SVLP-протокола
 * Из входных данных, формирует SVLP-сообщение и отправляет его
 *
 */

class SVLP_Writer : public Writer
{
public:
    /* Конструктор
     * args:
     * socket - сокет для чтения входящих данных
     * crc_enable - включение контрольной суммы в передаваемые сообщения
     */
    SVLP_Writer(ABstractSocket& socket, bool crc_enable);

    /*
     * Метод отправки SVLP-сообщения
     * args:
     * msg - сообщение созданное с помощью функции create_svlp_message(...)
     * ret:
     * колличество отправленных байт
     */
    uint32_t send_message(const svlp_message& msg, Command* cmd = 0, uint32_t timeout = 100);

    bool get_is_crc_enable() const;
    void set_is_crc_enable(bool value);

    /*
     * Метод создания SVLP-сообщения
     * info_code - infocode сообщения
     * msg_code - код команды
     * param - параметры команды
     * len - размер параметров в байтах
     *
     * ret:
     * svlp-сообщение
    */
    svlp_message create_svlp_message(const uint8_t info_code,
        const uint8_t msg_code,
        const void* params,
        const size_t len);
    svlp_message create_svlp_reply(command_uid_t cmd_uid,
        const uint8_t msg_code,
        const void* params,
        const size_t len);

    svlp_message create_svlp_message(const uint8_t info_code,
        const uint8_t msg_code)
    {
        return create_svlp_message(info_code, msg_code, 0, 0);
    }
    svlp_message create_svlp_reply(command_uid_t cmd_uid,
        const uint8_t msg_code)
    {
        return create_svlp_reply(cmd_uid, msg_code, 0, 0);
    }

    template <typename T>
    svlp_message create_svlp_message(const uint8_t info_code,
        const uint8_t msg_code,
        const T& params)
    {
        //STATIC_CHECK(!is_pointer<T>::value);
        return create_svlp_message(info_code, msg_code, &params, sizeof(T));
    }

    template <typename T>
    svlp_message create_svlp_reply(command_uid_t cmd_uid,
        const uint8_t msg_code,
        const T& params)
    {
        //STATIC_CHECK(!is_pointer<T>::value);
        return create_svlp_reply(cmd_uid, msg_code, &params, sizeof(T));
    }

    template <typename T, int N>
    svlp_message create_svlp_message(const uint8_t info_code,
        const uint8_t msg_code,
        const T (&params)[N])
    {
        return create_svlp_message(info_code, msg_code, params, sizeof(T) * N);
    }

    template <typename T, int N>
    svlp_message create_svlp_reply(command_uid_t cmd_uid,
        const uint8_t msg_code,
        const T (&params)[N])
    {
        return create_svlp_reply(cmd_uid, msg_code, params, sizeof(params));
    }

    virtual ~SVLP_Writer() {}

    uint32_t get_shot_message_size() const;
    void set_shot_message_size(const uint32_t& value);

    CommandMonitor* getCmdMonitor() const;
    void setCmdMonitor(CommandMonitor* value);

protected:
    /*
     * Вычисление контрольной суммы crc32
     * args:
     * buf - указатель на данные, для которых
     *      необходимо сосчитать контрольную сумму
     * size - размер данных, для которых
     *      необходимо сосчитать контрольную сумму
     * crc - текущее значение суммы
     *
     * Notice:
     * При последовательном вычислении контрольной суммы,
     *      размер всех порций данных, кроме последней
     *      должны быть кратны 4. Т.е.
     *      16, 32, 1024, 15 - Хорошо. Размер всех порций кроме последней, кратны 4.
     *      16, 32, 15, 512  - Плохо. Размер третьей порции данных 15 - байт
     *          не кратен 4.
     *
     */
    virtual uint32_t calc_crc(const void* buf, const size_t size, uint32_t crc = 0) const;

    /*
     * Отправляет данные до тех пор, пока они не будут отправлены они все или
     * количество попыток не превысит максимальное.
     * args:
     * data - указатель на данные, которые необходимо отправить
     * len - размер данных
     *
     */
    uint32_t forced_write(const void* data, uint32_t len);

    /*
     * Отправка сообщения за один заход.
     * Ранее отправка выполнялась в 2 (ещё ранее в 3) вызова socket.write()
     * Что приводило к повышению накладных расходорв на отправку сообщения.
     * Данная функция отправляет сообщение за один вызов функции socket.write()
     * !!!ВНИМАНИЕ!!!
     *  Данная функция сохраняет сообщение вместе со всеми параметрами на стеке,
     *  что может приводить к переполнению стека при попытке отправить сообщение большого размера.
     * !!Я предупредил!!
     * Во всём остальном интерфейс соответствует интерфейсу функции send_message(svlp_message msg)
     *
     *
     */
    uint32_t send_shot_message(const svlp_message& msg);

    /*
     * Отправка сообщения за 2(3) захода.
     * Интерфейс соответствует интерфейсу функции send_message(svlp_message msg)
     *
     */
    uint32_t send_long_message(const svlp_message& msg);

private:
    /*
     * рассчёт контрольной суммы
     * true - контрольная сумма рассчитывается
     * false - контрольная сумма не рассчитывается
     * методы доступа:
     *     bool getIs_crc_enable() const;
     *     void setIs_crc_enable(bool value);
     */
    bool is_crc_enable;
    uint16_t uid;

    /*
     * Для оптимизации накладных расходов, при отправке SVLP-сообщения,
     * Короткие SVLP-сообщения отправляются за один вызов socket.write(),
     * однако это требует бОльших объёмов памяти на стеке, пропорционально размеру сообщения.
     *
     * Максимальный размер сообщения отправляемого за один заход определяется значением shot_message_size
     * По умолчанию shot_message_size равен 0, что означает, что все SVLP-сообщения отправляются за 2 (за 3 при вклёченном crc)
     * вызова socket.write().
     * Методы доступа:
     *     uint32_t get_shot_message_size() const;
     *     void set_shot_message_size(const uint32_t& value);
     *
     */
    uint32_t shot_message_size;

    /*
     * Для обработки ответа на команду используется класс монитора команд(CommandMonitor).
     *  Если для SVLP_writer'а установлен монитора команд, то при отправке команды SVLP_writer
     *  будет сигнализировать монитору об отправленной команде.
     * Методы доступа:
     *     CommandMonitor* getCmdMonitor() const;
     *     void setCmdMonitor(CommandMonitor* value);
     *
     *
     */
    CommandMonitor* cmdMonitor;
};
}
#endif // SVLP_WRITER_H
