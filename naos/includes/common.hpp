/// \file common.hpp
/// \brief Head file for general definition.
///
/// Macro and type definition.
///
/// \author Kadds

#pragma once
#include <cstddef>
#include <cstdint>

#ifdef _MSC_VER
#error Not support msvc.
#endif

#if defined __GNUC__ || defined __clang__
#define ExportC extern "C"
#endif

#define NoReturn [[noreturn]]
#define PackStruct __attribute__((packed))
#define Aligned(v) __attribute__((aligned(v)))
#define Section(v) __attribute__((section(v)))

// type defs
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

/// easy use likely or unlikely
#ifndef likely
#define likely(x) __builtin_expect(!!(x), 1)
#endif
#ifndef unlikely
#define unlikely(x) __builtin_expect(!!(x), 0)
#endif

/// C++17 byte
typedef std::byte byte;
typedef u64 flag_t;

/// C++ init function
typedef void (*InitFunc)(void);
/// define by compiler
ExportC InitFunc __init_array_start;
ExportC InitFunc __init_array_end;

/// Initialize for c++ global variables
static inline void static_init()
{
    InitFunc *pFunc = &__init_array_start;

    for (; pFunc < &__init_array_end; ++pFunc)
    {
        (*pFunc)();
    }
}
