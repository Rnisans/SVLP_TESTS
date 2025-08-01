#ifndef MONITOR_FUTURE_H
#define MONITOR_FUTURE_H
#include "command.h"
#include "tuple"

namespace svlp
{

enum Result
{
    none,
    success,
    fail,
    timeout
};
template <class T>
class Monitor_future : public Command
{

    //расширить на результат несовпадения размера запрошенного параметра

    // Command interface
public:
    virtual void onSuccess(void* const param, const size_t param_len, command_uid_t cmd_uid) override
    {
    	(void)param_len;
    	(void)cmd_uid;
        value = *static_cast<T*>(param);
        result = success;
    }
    virtual void onFail(void* const param, const size_t param_len, command_uid_t cmd_uid) override
    {
    	(void)param_len;
    	(void)cmd_uid;
        value = *static_cast<T*>(param);
        result = fail;
    }
    virtual bool onProgress(void* const param, const size_t param_len, command_uid_t cmd_uid) override
    {
    	(void)param;
    	(void)param_len;
    	(void)cmd_uid;
    	return false;
    }
    virtual void onTimeout(command_uid_t cmd_uid) override
    {
    	(void)cmd_uid;
        result = timeout;
    }
    virtual void onUnsupportCommand(void* const param, const size_t param_len, command_uid_t cmd_uid) override
    {
    	(void)param;
    	(void)param_len;
    	(void)cmd_uid;
    }
    virtual void onReplace(command_uid_t cmd_uid) override
    {
    	(void)cmd_uid;
    }
    virtual bool onReply(uint8_t msg_code, void* const param, const size_t param_len, command_uid_t cmd_uid) override
    {
    	(void)msg_code;
    	(void)param;
    	(void)param_len;
    	(void)cmd_uid;
    	return false;
    }

    std::tuple<Result, T> get()
    {
        while (result == none) //Пока нет никакого результата
            ;
        return { result, value };
    }

private:
    volatile Result result = none;
    T value;
};
}

#endif // MONITOR_FUTURE_H
