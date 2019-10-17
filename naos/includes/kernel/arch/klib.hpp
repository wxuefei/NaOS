#pragma once
#include "../kernel.hpp"
#include "common.hpp"
#include "kernel/arch/idt.hpp"
#include "task.hpp"

ExportC void _reload_segment(u64 cs, u64 ss);

ExportC void _cpu_id(u64 param, u32 *out_eax, u32 *out_ebx, u32 *out_ecx, u32 *out_edx);
#define cpu_id_ex(eax, ecx, o_eax, o_ebx, o_ecx, o_edx)                                                                \
    _cpu_id((ecx) << 32 & (u32)(eax), (o_eax), (o_ebx), (o_ecx), (o_edx))
#define cpu_id(eax, o_eax, o_ebx, o_ecx, o_edx) _cpu_id((u32)(eax), (o_eax), (o_ebx), (o_ecx), (o_edx))

ExportC NoReturn void _kernel_thread(arch::task::regs_t *regs);
ExportC NoReturn void _switch_task(arch::task::register_info_t *prev, arch::task::register_info_t *next,
                                   task::thread_t *next_thd);

ExportC void _unpaged_reload_segment(u64 cs, u64 ss);
ExportC void _unpaged_reload_kernel_virtual_address(u64 offset);

inline void _wrmsr(u64 address, u64 value)
{
    __asm__ __volatile__("wrmsr  \n\t"
                         :
                         : "d"((u32)(value >> 32)), "a"((u32)(value & 0xFFFFFFFF)), "c"(address)
                         : "memory");
}
inline u64 _rdmsr(u64 address)
{
    u32 v0 = 0;
    u32 v1 = 0;
    __asm__ __volatile__("rdmsr	\n\t" : "=d"(v0), "=a"(v1) : "c"(address) : "memory");
    return ((u64)v0) << 32 | v1;
}

inline u64 _rdtsc()
{
    u32 v0, v1;
    __asm__ __volatile__("mfence \n\t rdtsc	\n\t" : "=d"(v0), "=a"(v1) : : "memory");
    return ((u64)v0) << 32 | v1;
}

inline void _mfence() { __asm__ __volatile__("mfence\n\t" : : : "memory"); }

ExportC volatile char _sys_call;
ExportC volatile char _sys_ret;

ExportC NoReturn void _call_sys_ret(arch::task::regs_t *regs);
ExportC void _switch_stack(u64 param1, u64 param2, u64 param3, u64 param4, void *func, void *rsp);

inline static u64 get_stack()
{
    u64 v0;
    __asm__ __volatile__("movq %%rsp, %0 \n\t" : "=r"(v0) : : "memory");
    return v0;
}

void *print_stack(const arch::idt::regs_t *regs, int max_depth);
