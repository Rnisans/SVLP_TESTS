#ifndef __TYPE_TRAITS__
#define __TYPE_TRAITS__

template <typename T>
struct is_pointer
{
    static const bool value = false;
};

template <typename U>
struct is_pointer<U*>
{
    static const bool value = true;
};

#endif //__TYPE_TRAITS__
