#pragma once
#include "arch/vbe.hpp"
#include "common.hpp"
namespace trace
{
extern bool output_debug;
template <typename... Args> NoReturn void panic(const Args &... args)
{
    device::vbe::print("[panic] ", args...);
    device::vbe::print('\n');
    while (1)
        ;
}
template <typename... Args> void info(const Args &... args)
{
    device::vbe::print("[info] ", args...);
    device::vbe::print('\n');
}

template <typename... Args> void debug(const Args &... args)
{
    if (!output_debug)
        return;
    device::vbe::print("[debug] ", args...);
    device::vbe::print('\n');
}

template <typename... Args> void assert_runtime(const char *exp, const char *file, int line, const Args &... args)
{
    device::vbe::print("[assert] runtime assert failed: at: ", file, ':', line, "\n    assert expr: ", exp, '\n');
    panic("from assert failed. ", args...);
}

} // namespace trace

#ifndef NDEBUG
#define kassert(exp, ...)                                                                                              \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(exp))                                                                                                    \
        {                                                                                                              \
            (trace::assert_runtime(#exp, __FILE__, __LINE__, __VA_ARGS__));                                            \
        }                                                                                                              \
    } while (0)
#else
#define kassert(exp, ...) void(0)
#endif