#ifndef CALLBACK_H_
#define CALLBACK_H_

#include "callback_abstract.h"

template <typename T>
class Callback;

// Класс Callback - шаблонный клас-обёртка над методами классов. Используется для реализации софтварного таймера.
//  позволяет хранить указатель на метод класса и вызывать его.
//  Поддерживает методы с различными возвращаемыми типами.
//  Не поддерживает методы с параметрами.
// В качестве параметра шаблона принимает тип указателя на метод класса.

template <typename Return, typename Object>
class Callback<Return (Object::*)()> : public Callback_Abstract
{
public:
    // Конструктор класса, принимает указатель на метод, и ссылку на объект класса
    // ВАЖНО! Необходимо следить, чтобы время жизни объекта было не короче, чем время жизни объекта-callback.
    //      вызов функции run(), после того, как объетк, на который ссылается i_obj был удалён, приведёт к неопределённому поведению.

    // param:
    //  i_method_ptr - указатель на метод класса
    //  i_obj - ссылка на объект класса, к которому пренадлежит метод i_method_ptr.
    Callback(Return (Object::*i_method_ptr)(), Object& i_obj)
        : method_ptr(i_method_ptr)
        , obj(i_obj)
    {
    }

    virtual ~Callback()
    {
    }

    // вызов хранимого метода класса
    void run()
    {
        // вызов метода method_ptr объекта obj
        (obj.*method_ptr)();
        return;
    }

private:
    // Указатель на метод класса
    Return (Object::*method_ptr)();
    // ссылка на объект класса, к которому пренадлежит метод method_ptr.
    Object& obj;
};

template <typename Return, typename Object, class Prm1>
class Callback<Return (Object::*)(Prm1)> : public Callback_Abstract1<Prm1>
{
public:
    // Конструктор класса, принимает указатель на метод, и ссылку на объект класса
    // ВАЖНО! Необходимо следить, чтобы время жизни объекта было не короче, чем время жизни объекта-callback.
    //      вызов функции run(), после того, как объетк, на который ссылается i_obj был удалён, приведёт к неопределённому поведению.

    // param:
    //  i_method_ptr - указатель на метод класса
    //  i_obj - ссылка на объект класса, к которому пренадлежит метод i_method_ptr.
    Callback(Return (Object::*i_method_ptr)(Prm1), Object& i_obj)
        : method_ptr(i_method_ptr)
        , obj(i_obj)
    {
    }

    virtual ~Callback()
    {
    }

    // вызов хранимого метода класса
    void run(const Prm1& param)
    {
        // вызов метода method_ptr объекта obj
        (obj.*method_ptr)(param);
        return;
    }

private:
    // Указатель на метод класса
    Return (Object::*method_ptr)(Prm1);
    // ссылка на объект класса, к которому пренадлежит метод method_ptr.
    Object& obj;
};

template <typename Return, typename Object, class Prm1, class Prm2>
class Callback<Return (Object::*)(Prm1, Prm2)> : public Callback_Abstract2<Prm1, Prm2>
{
public:
    // Конструктор класса, принимает указатель на метод, и ссылку на объект класса
    // ВАЖНО! Необходимо следить, чтобы время жизни объекта было не короче, чем время жизни объекта-callback.
    //      вызов функции run(), после того, как объетк, на который ссылается i_obj был удалён, приведёт к неопределённому поведению.

    // param:
    //  i_method_ptr - указатель на метод класса
    //  i_obj - ссылка на объект класса, к которому пренадлежит метод i_method_ptr.
    Callback(Return (Object::*i_method_ptr)(Prm1, Prm2), Object& i_obj)
        : method_ptr(i_method_ptr)
        , obj(i_obj)
    {
    }

    virtual ~Callback()
    {
    }

    // вызов хранимого метода класса
    void run(const Prm1& param, const Prm2& param2)
    {
        // вызов метода method_ptr объекта obj
        (obj.*method_ptr)(param, param2);
        return;
    }

private:
    // Указатель на метод класса
    Return (Object::*method_ptr)(Prm1, Prm2);
    // ссылка на объект класса, к которому пренадлежит метод method_ptr.
    Object& obj;
};



template <typename Return, typename Object, class Prm1, class Prm2, class Prm3>
class Callback<Return (Object::*)(Prm1,  Prm2,  Prm3)> : public Callback_Abstract3<Prm1, Prm2, Prm3>
{
public:
    // Конструктор класса, принимает указатель на метод, и ссылку на объект класса
    // ВАЖНО! Необходимо следить, чтобы время жизни объекта было не короче, чем время жизни объекта-callback.
    //      вызов функции run(), после того, как объетк, на который ссылается i_obj был удалён, приведёт к неопределённому поведению.

    // param:
    //  i_method_ptr - указатель на метод класса
    //  i_obj - ссылка на объект класса, к которому пренадлежит метод i_method_ptr.
    Callback(Return (Object::*i_method_ptr)(Prm1, Prm2, Prm3), Object& i_obj)
        : method_ptr(i_method_ptr)
        , obj(i_obj)
    {
    }

    virtual ~Callback()
    {
    }

    // вызов хранимого метода класса
    void run(const Prm1& param, const Prm2& param2, const Prm3& param3)
    {
        // вызов метода method_ptr объекта obj
        (obj.*method_ptr)(param, param2, param3);
        return;
    }

private:
    // Указатель на метод класса
    Return (Object::*method_ptr)(Prm1, Prm2, Prm3);
    // ссылка на объект класса, к которому пренадлежит метод method_ptr.
    Object& obj;
};


template <typename Return, typename Object, class Prm1, class Prm2, class Prm3, class Prm4>
class Callback<Return (Object::*)(Prm1, Prm2,Prm3, Prm4)> : public Callback_Abstract4<Prm1, Prm2, Prm3, Prm4>
{
public:
    // Конструктор класса, принимает указатель на метод, и ссылку на объект класса
    // ВАЖНО! Необходимо следить, чтобы время жизни объекта было не короче, чем время жизни объекта-callback.
    //      вызов функции run(), после того, как объетк, на который ссылается i_obj был удалён, приведёт к неопределённому поведению.

    // param:
    //  i_method_ptr - указатель на метод класса
    //  i_obj - ссылка на объект класса, к которому пренадлежит метод i_method_ptr.
    Callback(Return (Object::*i_method_ptr)(Prm1, Prm2,  Prm3,  Prm4), Object& i_obj)
        : method_ptr(i_method_ptr)
        , obj(i_obj)
    {
    }

    virtual ~Callback()
    {
    }

    // вызов хранимого метода класса
    void run(const Prm1& param, const Prm2& param2, const Prm3& param3, const Prm4& param4) override
    {
        // вызов метода method_ptr объекта obj
        (obj.*method_ptr)(param, param2, param3, param4);
        return;
    }

private:
    // Указатель на метод класса
    Return (Object::*method_ptr)( Prm1, Prm2, Prm3,  Prm4);
    // ссылка на объект класса, к которому пренадлежит метод method_ptr.
    Object& obj;
};


template <typename Return, typename Object, class Prm1, class Prm2, class Prm3, class Prm4, class Prm5>
class Callback<Return (Object::*)(Prm1,  Prm2, Prm3, Prm4, Prm5)> : public Callback_Abstract5<Prm1, Prm2,Prm3, Prm4, Prm5>
{
public:
    // Конструктор класса, принимает указатель на метод, и ссылку на объект класса
    // ВАЖНО! Необходимо следить, чтобы время жизни объекта было не короче, чем время жизни объекта-callback.
    //      вызов функции run(), после того, как объетк, на который ссылается i_obj был удалён, приведёт к неопределённому поведению.

    // param:
    //  i_method_ptr - указатель на метод класса
    //  i_obj - ссылка на объект класса, к которому пренадлежит метод i_method_ptr.
    Callback(Return (Object::*i_method_ptr)(Prm1, Prm2, Prm3,  Prm4, Prm5), Object& i_obj)
        : method_ptr(i_method_ptr)
        , obj(i_obj)
    {
    }

    virtual ~Callback()
    {
    }

    // вызов хранимого метода класса
    void run(const Prm1& param, const Prm2& param2,const Prm3& param3, const Prm4& param4,const Prm5& param5)
    {
        // вызов метода method_ptr объекта obj
        (obj.*method_ptr)(param, param2, param3, param4, param5);
        return;
    }

private:
    // Указатель на метод класса
    Return (Object::*method_ptr)(Prm1,  Prm2,  Prm3,  Prm4, Prm5);
    // ссылка на объект класса, к которому пренадлежит метод method_ptr.
    Object& obj;
};

//Класс Calback для статических и свободных функций
//template <class Return>
//class Callback<Return (*)(void* context)> : public Callback_Abstract
//{
//public:

//    // Конструктор класса, принимает указатель на свободную или статическую функцию и указатель на контекст
//    // ВАЖНО! Необходимо следить, чтобы время жизни контекста было не короче, чем время жизни объекта-callback.
//    //      вызов функции run(), после того, как контекста, на который ссылается context был удалён, приведёт к неопределённому поведению.

//    Callback(Return (*my_memfunc_ptr)(void*), void* context)
//        : my_memfunc_ptr(my_memfunc_ptr)
//        , context(context)
//    {
//    }

//    virtual ~Callback() {}

//    void run()
//    {
//        (my_memfunc_ptr)(context);
//    }

//    Return (*my_memfunc_ptr)(void* context);
//    void* context;
//};


    template <class Return, class Object, class Prm1, class Prm2>
    Callback_Abstract * make_callback(Return (Object::*i_method_ptr)(const Prm1&, const Prm2&), Object& i_obj)
    {
        return Callback<Return(Object::*)(Prm1 p1, Prm2 p2)>(i_method_ptr);
    }

#endif /* CALLBACK_H_ */
