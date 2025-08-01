#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H

class NonCopyable
{
public:
    NonCopyable() {}

//Имеет смысл только если компилятор поддерживает с++11 и выше
#if __cplusplus >= 201103L
    NonCopyable(NonCopyable&&) = default;
    NonCopyable& operator=(NonCopyable&&) = default;
#endif

private:
    NonCopyable(const NonCopyable& other);
    NonCopyable& operator=(const NonCopyable& other);
};

#endif // NONCOPYABLE_H
