#ifndef NONMOVABLE_H
#define NONMOVABLE_H
class NonMovable
{
//Имеет смысл только если компилятор поддерживает с++11 и выше
#if __cplusplus >= 201103L
public:
    NonMovable() {}

    NonMovable(const NonMovable& other) = default;
    NonMovable& operator=(const NonMovable& other) = default;

private:
    NonMovable(NonMovable&&);
    NonMovable& operator=(NonMovable&&);
#endif
};
#endif // NONMOVABLE_H
