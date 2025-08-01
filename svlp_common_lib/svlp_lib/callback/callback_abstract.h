#ifndef CALLBACK_ABSTRACT_H_
#define CALLBACK_ABSTRACT_H_


template<class Param1, class Param2, class Param3, class Param4, class Param5>
class Callback_Abstract5 {
    public:
        Callback_Abstract5() {};
        //Если есть хоть один виртуальный метод, деструктор тоже должен быть virtual
        virtual ~Callback_Abstract5() {};
        virtual void run(const Param1& param, const Param2& param2, const Param3& param3, const Param4& param4, const Param5& param5) = 0;
};

template<class Param1, class Param2, class Param3, class Param4>
class Callback_Abstract4 {
    public:
        Callback_Abstract4() {};
        //Если есть хоть один виртуальный метод, деструктор тоже должен быть virtual
        virtual ~Callback_Abstract4() {};
        virtual void run(const Param1& param, const Param2& param2,const Param3& param3, const Param4& param4) = 0;
};

template<class Param1, class Param2, class Param3>
class Callback_Abstract3 {
    public:
        Callback_Abstract3() {};
        //Если есть хоть один виртуальный метод, деструктор тоже должен быть virtual
        virtual ~Callback_Abstract3() {};
        virtual void run(const Param1& param, const Param2& param2, const Param3& param3) = 0;
};


template<class Param1, class Param2>
class Callback_Abstract2 {
    public:
        Callback_Abstract2() {};
        //Если есть хоть один виртуальный метод, деструктор тоже должен быть virtual
        virtual ~Callback_Abstract2() {};
        virtual void run(const Param1& param, const Param2& param2) = 0;
};

template<class Param1>
class Callback_Abstract1 {
    public:
        Callback_Abstract1() {};
        //Если есть хоть один виртуальный метод, деструктор тоже должен быть virtual
        virtual ~Callback_Abstract1() {};
        virtual void run(const Param1& param) = 0;
};

class Callback_Abstract {
    public:
        Callback_Abstract() {};
        //Если есть хоть один виртуальный метод, деструктор тоже должен быть virtual
        virtual ~Callback_Abstract() {};
        virtual void run() = 0;
};

#endif /* CALLBACK_ABSTRACT_H_ */
