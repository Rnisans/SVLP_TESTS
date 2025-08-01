#ifndef SVLP_PARSER_H
#define SVLP_PARSER_H

#include "../crc/crc32_apple.h"
#include "handlers_stores/simple_handler_store.h"
#include "parser.h"
#include "stdint.h"
#include "svlp_filter/abstract_filter.h"
#include "svlp_header.h"
#include <stddef.h>

//FIX IT
#ifndef assert
#define assert(expression) void(0)
#endif

namespace svlp
{
class CommandMonitor;

static inline size_t min(size_t a, size_t b)
{
    if (a < b)
        return a;
    else
        return b;
}

static inline bool is_paramless_header(const svlp_message_header& header)
{
    return !(header.params_length);
}

static inline bool is_crcless_header(const svlp_message_header& header)
{
    return !(header.flags & is_crc_mask);
}

/*
 * Класс парсера SVLP протокола
 */
template <int buffer_size, class Handlers_store_t = Simple_handler_store>
class Basic_SVLP_Parser : public Parser
{
public:
    typedef typename Handlers_store_t::uid_t Handler_id_t;
    /*Конструктор
     * args:
     * socket - сокет для чтения входящих данных
     */

    explicit Basic_SVLP_Parser(ABstractSocket* socket)
        : Parser(socket)
        , max_command_count(20)
        , cmdMonitor(0)
        //        , commands(Handlers_store_t())
        , loger(0)
        , totaly_read(0)
        , message_count(0)
        , broken_message_count(0)
        , reader_state(smvr_wait_sof1)
        , crc_check(true)
        , max_packet_len(buffer_size)
        , filter(0)
        , current_position(0)

    {
        current_msg.params = data;
    }

    virtual bool remove_message_handler(Handler_id_t uid)
    {
        return commands.remove(uid);
    }

    virtual void clear_message_handlers(uint8_t infocode)
    {
        return commands.clear(infocode);
    }

    /*
     * Добавляет обработчик входящих команд для определённого infocode
     * Реализует шаблон проектирования "Команда".
     * Для удаления обработчика следует вызвать данный метод, передав в качестве
     * первого аргумента ноль.
     * args:
     * cmd - обработчик входящих команд
     * infocode - infocode сообщении, которые будет обрабатывать данный обработчик
     *
*/

    virtual Handler_id_t add_message_handler(AbstractMessageHandler* cmd, const uint8_t infocode)
    {
        return commands.insert(cmd, infocode);
    }

    /*
     * Чисто виртуальный метод
     * выполняет чтение и разбор сообщений
     * при необходимости проверяет контрольную суму сообщений
     * после разбора и проверки контрольной суммы передаёт
     *      полученную команду и параметры соответствующему обработчику
     *
     * Notice:
     * Данный метод возвращает управление, только после обработки всех данных в сокете или когда будет обработано max_command_count команд,
     *      по умолчанию  max_command_count = 20.
     * Обработчики команд так же выполняются в контексте этой функции
     *
     */
    uint32_t parse()
    {
        uint8_t tempch = 0;
        uint32_t comand_counter = 0;
        bool is_last_part = false;
        bool isFinished;
        while ((comand_counter <= max_command_count) && is_data_ready())
        {
            switch (reader_state)
            {
            //Ждём разделители - 0xAA 0xAA 0xAA 0xAA
            case smvr_wait_sof1:
                crc = 0;
                if (!read_data(&tempch, 1)) //если не удалось считать
                {
                    return comand_counter;
                }
                if (tempch == 0xAA)
                {
                    reader_state = smvr_wait_sof2;
                }
                else
                {
                    reader_state = smvr_wait_sof1;
                }
                break;
            case smvr_wait_sof2:
                if (!read_data(&tempch, 1)) //если не удалось считать
                {
                    return comand_counter;
                }
                if (tempch == 0xAA)
                {
                    reader_state = smvr_wait_sof3;
                }
                else
                {
                    reader_state = smvr_wait_sof1;
                }
                break;
            case smvr_wait_sof3:
                if (!read_data(&tempch, 1)) //если не удалось считать
                {
                    return comand_counter;
                }
                if (tempch == 0xAA)
                {
                    reader_state = smvr_wait_sof4;
                }
                else
                {
                    reader_state = smvr_wait_sof1;
                }
                break;
            case smvr_wait_sof4:
                if (!read_data(&tempch, 1)) //если не удалось считать
                {
                    return comand_counter;
                }
                if (tempch == 0xAA)
                {
                    reader_state = smvr_wait_header;
                }
                else
                {
                    reader_state = smvr_wait_sof1;
                }
                break;
            case smvr_wait_header:
                read_to_buffer(reinterpret_cast<uint8_t*>(&current_msg.header),
                    sizeof(current_msg.header),
                    &isFinished);
                if (isFinished)
                {
                    if (is_suported_SVLP_header(current_msg.header)) //по протоколу поле recerve должно быть пустым
                    {
                        if (is_paramless_header(current_msg.header))
                        {
                            if (!crc_check || is_crcless_header(current_msg.header))
                            {
                                perform_cmd(current_msg);
                                comand_counter++;
                                reader_state = smvr_wait_sof1;
                            }
                            else
                            {

                                crc = calc_crc(&current_msg.header, sizeof(current_msg.header));
                                reader_state = smvr_wait_crc;
                            }
                        }
                        else
                        {
                            if (is_crcless_header(current_msg.header))
                            {
                                reader_state = smvr_wait_data;
                            }
                            else
                            {
                                if (crc_check)
                                {
                                    crc = calc_crc(&current_msg.header, sizeof(current_msg.header));
                                }
                                reader_state = smvr_wait_data;
                            }
                        }
                    }
                    else
                    {
                        reader_state = smvr_wait_sof1;
                    }
                    totaly_read = 0;
                    current_position = 0;
                }
                break;
            case smvr_wait_data:
            {
                read_to_buffer(static_cast<uint8_t*>(current_msg.params), current_msg.header.params_length, &isFinished);
                is_last_part = (totaly_read == current_msg.header.params_length);
                if (isFinished)
                {
                    stream_data(current_msg.header.infocode);
                    if (crc_check && !is_crcless_header(current_msg.header))
                    {
                        crc = calc_crc(data, current_position, crc);
                    }
                    if (is_last_part)
                    {
                        if (is_crcless_header(current_msg.header))
                        {
                            perform_cmd(current_msg);
                            comand_counter++;
                            reader_state = smvr_wait_sof1;
                        }
                        else
                        {
                            reader_state = smvr_wait_crc;
                        }
                        totaly_read = 0;
                    }
                    current_position = 0;
                }
                break;
            }
            case smvr_wait_crc:
                read_to_buffer(reinterpret_cast<uint8_t*>(&current_msg.crc), sizeof(current_msg.crc), &isFinished);
                if (isFinished)
                {
                    if (!crc_check || (current_msg.crc == crc))
                    {
                        perform_cmd(current_msg);
                        comand_counter++;
                    }
                    else
                    {
                        broken_message_count++;
                    }
                    reader_state = smvr_wait_sof1;
                    totaly_read = 0;
                    current_position = 0;
                }
                break;
            }
        }
        return comand_counter;
    }

    /* Метод, сбрасывает внутреннее состояние парсера в первоначальное.
     * Полезно, например после приёма кривого пакета.
     */
    void reset()
    {
        reader_state = smvr_wait_sof1;
        totaly_read = 0;
        current_position = 0;
    }
    /*
     * Устанавливает логер сырых данных
     * args:
     * loger - логер, для логирования "сырых" данных, полученных из сокета
     */
    void set_loger(Loger* loger)
    {
        this->loger = loger;
    }
    Loger* getLoger() const
    {
        return loger;
    }

    /*
     * Возвращает колличество полученных сообщений
     */
    uint32_t get_message_count() const
    {
        return message_count;
    }

    /*
     * Возвращает колличество полученных сообщений,
     *      для которых не сошлась контрольная сумма
     */
    uint32_t getBroken_message_count() const
    {
        return broken_message_count;
    }

    /*
     * crc_check - указывает на необходимость проверять контрольную сумму
     * crc_check == false - контрольная сумма не проверяется, команды с неверной
     *                      суммой будут выполнены!!!
     * crc_check == true - при наличии в сообщении контрольной суммы, происходит проверка,
     *                      команды с неверной контрольной суммой выполнены не будут.
     *
     * методы доступа:
     *      bool getCrc_check() const;
     *      void setCrc_check(bool enable);
     */
    bool get_crc_check() const
    {
        return crc_check;
    }
    void set_crc_check(bool enable)
    {
        crc_check = enable;
    }

    /*
     * Метод возвращает обработчик входящих команд,
     * установленный для определённого infocode. Если обраюотчик для данного infocode
     * установлен не был - возвращает нулевой указатель.
     * args:
     * uid - идентификатор обработчика, который вернула функция add_message_handler при добавлении
     * ret:
     * Указатель на обработчик сообщений
     *
     */
    AbstractMessageHandler* get_message_handler(const Handler_id_t uid) const
    {
        return commands.get(uid);
    }

    /*
     * Во избежание зависаний парсера при использовании ненадёжного канала,
     * следует ограничивать максимальный принимаемый размер пакета.
     * max_packet_len содержит максимальный размер пакета, подлежащего обработке.
     * Все пакеты размером более max_packet_len будут отброшены.
     * max_packet_len по умолчанию равен 256 байт.
     *
     * Методы доступа
     *     uint32_t getMax_packet_len() const;
     *     void setMax_packet_len(const uint32_t &value);
     *
     */
    uint32_t getMax_packet_len() const
    {
        return max_packet_len;
    }
    void setMax_packet_len(const uint32_t& value)
    {
        max_packet_len = value;
    }

    uint32_t getMax_command_count() const
    {
        return max_command_count;
    }
    void setMax_command_count(const uint32_t value)
    {
        max_command_count = value;
    }

    virtual ~Basic_SVLP_Parser() {}

    void set_filters(svlp::Abstract_filter* const filter)
    {
        this->filter = filter;
    }

    svlp::Abstract_filter* get_filter() const
    {
        return filter;
    }

    CommandMonitor* getCmdMonitor() const
    {
        return cmdMonitor;
    }

    void setCmdMonitor(CommandMonitor* value)
    {
        cmdMonitor = value;
        add_message_handler((AbstractMessageHandler* )value, infocode_cmd_result);
    }

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
     *      16, 32, 15, 512  - Плохо. Размер третьей порции данных 15 байт -
     *          не кратен 4.
     *
     */
    virtual uint32_t calc_crc(const void* buf, const size_t size, uint32_t crc = 0) const
    {
        return ComputeCRC32(crc, static_cast<const uint8_t*>(buf), size);
    }

    /*
     * Метод выполняет проверку соответствия
     *      header'а сообщения протоколу, поддерживаемумому парсером
     *
     * args:
     * header - заголовок сообщения, который необходимо проверить
     *
     * ret - true - Заголовок поддерживается парсером
     *       false - Заголовок не поддерживается парсером
     */
    virtual bool is_suported_SVLP_header(svlp_message_header header)
    {
        return (header.version == svlp_version) && //проверка версии протокола
            (header.params_length <= max_packet_len);
    }

private:
    //Максимальное колличество команд, выполняемых за один вызов SVLP_Parser::parse()
    uint32_t max_command_count;
    //Версия SVLP протокола
    static const int8_t svlp_version = 1;
    /*
     * Состояния конечного автомата
     */
    enum StateMachineSVLPReader
    {
        smvr_wait_sof1, //ждём первый разделитель
        smvr_wait_sof2, //ждём второй разделитель
        smvr_wait_sof3, //ждём первый разделитель
        smvr_wait_sof4, //ждём первый разделитель
        smvr_wait_header, //ждём заголовок
        smvr_wait_data, //ждём данные
        smvr_wait_crc //ждём crc
    };

    void log(Loger* loger, const svlp_message& data)
    {

        if (loger)
            loger->log(data);
    }

    bool perform_cmd(svlp_message& msg)
    {
        if (!filter || filter->check(msg.header.infocode, msg.header.msgcode))
        {
            log(loger, msg);
            message_count++;
            if (msg.header.params_length > buffer_len)
            {
                commands.perform_handlers(msg.header.infocode,
                    msg.header.msgcode, 0, 0,
                    msg.header.uid);
            }
            else
            {

                commands.perform_handlers(msg.header.infocode,
                    msg.header.msgcode,
                    data,
                    msg.header.params_length,
                    msg.header.uid);
            }
            return true;
        }
        return false;
    }

    size_t read_to_buffer(uint8_t* buffer, size_t len, bool* isFinal)
    {
        uint_fast32_t available_space = 0;
        uint32_t readed = 0;
        available_space = buffer_len - current_position;
        size_t byte_to_read = min(available_space, (len - totaly_read));
        readed = read_data(&(buffer[current_position]), byte_to_read);
        current_position += readed;
        totaly_read += readed;
        available_space = buffer_len - current_position;
        *isFinal = (totaly_read == len) || (!available_space);
        assert(totaly_read <= len);
        return readed;
    }
    bool stream_data(uint8_t infocode)
    {

        bool is_last_part = (totaly_read == current_msg.header.params_length);
        size_t cmd_performed = commands.perform_stream_handlers(infocode, current_msg.header.msgcode,
            data, current_position, is_last_part, current_msg.header.uid);
        return cmd_performed;
    }

    CommandMonitor* cmdMonitor;

    Handlers_store_t commands;
    Loger* loger;

    uint32_t totaly_read;
    uint32_t message_count;
    uint32_t broken_message_count;
    StateMachineSVLPReader reader_state;
    svlp_message current_msg;
    bool crc_check;
    uint32_t crc;
    uint32_t max_packet_len;

    static const size_t buffer_len = buffer_size;
    uint8_t data[buffer_size];

    Abstract_filter* filter;
    size_t current_position;
};

typedef Basic_SVLP_Parser<256> SVLP_Parser;
typedef Basic_SVLP_Parser<2560> SVLP_Parser_TV;

}
#endif // SVLP_PARSER_H
