#ifndef __STATIC_ASSERT__
#define __STATIC_ASSERT__

template <bool>
struct compile_time_assert;
template <>
struct compile_time_assert<true>
{
};

#define STATIC_CHECK(expr) (compile_time_assert<(expr) != 0>())

#endif //__STATIC_ASSERT__
