#include "kernel/arch/exception.hpp"
#include "kernel/arch/cpu.hpp"
#include "kernel/arch/idt.hpp"
#include "kernel/kernel.hpp"
#include "kernel/trace.hpp"
#include "kernel/ucontext.hpp"

namespace arch::exception
{
using idt::regs_t;
arch::idt::call_func global_call_func = 0;
arch::idt::call_func global_exit_call_func = 0;

ExportC u64 _get_cr2();

ExportC void _divide_error_wrapper();
ExportC void _debug_wrapper();
ExportC void _nmi_wrapper();
ExportC void _int3_wrapper();

ExportC void _overflow_wrapper();
ExportC void _bounds_wrapper();
ExportC void _undefined_opcode_wrapper();
ExportC void _dev_not_available_wrapper();
ExportC void _double_fault_wrapper();
ExportC void _coprocessor_segment_overrun_wrapper();
ExportC void _invalid_TSS_wrapper();
ExportC void _segment_not_present_wrapper();
ExportC void _stack_segment_fault_wrapper();
ExportC void _general_protection_wrapper();
ExportC void _page_fault_wrapper();
ExportC void _x87_FPU_error_wrapper();
ExportC void _alignment_check_wrapper();
ExportC void _machine_check_wrapper();
ExportC void _SIMD_exception_wrapper();
ExportC void _virtualization_exception_wrapper();

void print_dst(regs_t *regs) { trace::debug("exception at: ", (void *)regs->rip, ", error code: ", regs->error_code); }

void _ctx_interrupt_ dispatch_exception(regs_t *regs)
{
    u64 extra_data = 0;
    if (regs->vector == 14)
    {
        __asm__ __volatile__("movq %%cr2, %0	\n\t" : "=r"(extra_data) : : "memory");
    }
    bool handled = false;
    if (likely(global_call_func))
        handled = global_call_func(regs, extra_data);
    if (!handled)
    {
        auto &cpu = cpu::current();
        if (cpu.has_task() && cpu.is_in_user_context((void *)regs->rsp))
        {
            cpu.get_task()->register_info->trap_vector = regs->vector;
        }

        else if (cpu.is_in_kernel_context((void *)regs->rsp) || cpu.is_in_hard_irq_context((void *)regs->rsp) ||
                 cpu.is_in_exception_context((void *)regs->rsp))
        {
            if (cpu.get_task() != nullptr)
            {
                cpu.get_task()->register_info->trap_vector = regs->vector;
            }
            trace::panic_stack(regs, "Oops error");
        }
    }

    if (likely(global_exit_call_func))
    {
        global_exit_call_func(regs, extra_data);
    }
}

ExportC _ctx_interrupt_ void entry_divide_error(regs_t *regs) { trace::debug("divide error. "); }

ExportC _ctx_interrupt_ void entry_debug(regs_t *regs) { trace::debug("debug trap. "); }

ExportC _ctx_interrupt_ void entry_nmi(regs_t *regs)
{
    trace::debug("nmi interrupt! ");
    print_dst(regs);
    dispatch_exception(regs);
}

ExportC _ctx_interrupt_ void entry_int3(regs_t *regs) { trace::debug("int3 trap. "); }

ExportC _ctx_interrupt_ void entry_overflow(regs_t *regs) { trace::debug("overflow trap. "); }

ExportC _ctx_interrupt_ void entry_bounds(regs_t *regs) { trace::debug("out of bounds error. "); }

ExportC _ctx_interrupt_ void entry_undefined_opcode(regs_t *regs) { trace::debug("undefined opcode error. "); }

ExportC _ctx_interrupt_ void entry_dev_not_available(regs_t *regs) { trace::debug("dev not available error. "); }

ExportC _ctx_interrupt_ void entry_double_fault(regs_t *regs) { trace::debug("double abort. "); }

ExportC _ctx_interrupt_ void entry_coprocessor_segment_overrun(regs_t *regs)
{
    trace::debug("coprocessor segment overrun error. ");
}
ExportC _ctx_interrupt_ void entry_invalid_TSS(regs_t *regs) { trace::debug("invalid tss error. "); }

ExportC _ctx_interrupt_ void entry_segment_not_present(regs_t *regs) { trace::debug("segment not present error. "); }

ExportC _ctx_interrupt_ void entry_stack_segment_fault(regs_t *regs) { trace::debug("stack segment fault. "); }

ExportC _ctx_interrupt_ void entry_general_protection(regs_t *regs) { trace::debug("general protection error. "); }

ExportC _ctx_interrupt_ void entry_page_fault(regs_t *regs)
{
    trace::debug("page fault. ");
    u64 cr2;
    __asm__ __volatile__("movq %%cr2, %0	\n\t" : "=r"(cr2) : : "memory");

    trace::debug("page at: ", (void *)cr2);
}

ExportC _ctx_interrupt_ void entry_x87_FPU_error(regs_t *regs) { trace::debug("X87 fpu error. "); }

ExportC _ctx_interrupt_ void entry_alignment_check(regs_t *regs) { trace::debug("alignment error. "); }

ExportC _ctx_interrupt_ void entry_machine_check(regs_t *regs) { trace::debug("machine error. "); }

ExportC _ctx_interrupt_ void entry_SIMD_exception(regs_t *regs) { trace::debug("SIMD error. "); }

ExportC _ctx_interrupt_ void entry_virtualization_exception(regs_t *regs) { trace::debug("virtualization error. "); }
typedef void (*entry_func)(regs_t *regs);

entry_func exceptions[] = {
    entry_divide_error,
    entry_debug,
    entry_nmi,
    entry_int3,
    entry_overflow,
    entry_bounds,
    entry_undefined_opcode,
    entry_dev_not_available,
    entry_double_fault,
    entry_coprocessor_segment_overrun,
    entry_invalid_TSS,
    entry_segment_not_present,
    entry_stack_segment_fault,
    entry_general_protection,
    entry_page_fault,
    (entry_func)0,
    entry_x87_FPU_error,
    entry_alignment_check,
    entry_machine_check,
    entry_SIMD_exception,
    entry_virtualization_exception,
};
ExportC _ctx_interrupt_ void __do_exception(regs_t *regs)
{
    exceptions[regs->vector](regs);
    dispatch_exception(regs);
}

void init()
{
    using namespace idt;
    set_interrupt_gate(0, (void *)_divide_error_wrapper, 0);
    set_interrupt_system_gate(1, (void *)_debug_wrapper, 3);
    set_interrupt_system_gate(2, (void *)_nmi_wrapper, 4);
    set_interrupt_gate(3, (void *)_int3_wrapper, 0);
    set_interrupt_gate(4, (void *)_overflow_wrapper, 0);
    set_interrupt_gate(5, (void *)_bounds_wrapper, 0);
    set_interrupt_system_gate(6, (void *)_undefined_opcode_wrapper, 0);
    set_interrupt_system_gate(7, (void *)_dev_not_available_wrapper, 0);
    set_interrupt_system_gate(8, (void *)_double_fault_wrapper, 3);
    set_interrupt_system_gate(9, (void *)_coprocessor_segment_overrun_wrapper, 0);
    set_interrupt_system_gate(10, (void *)_invalid_TSS_wrapper, 0);
    set_interrupt_system_gate(11, (void *)_segment_not_present_wrapper, 0);
    set_interrupt_system_gate(12, (void *)_stack_segment_fault_wrapper, 0);
    set_interrupt_system_gate(13, (void *)_general_protection_wrapper, 0);
    set_interrupt_system_gate(14, (void *)_page_fault_wrapper, 0);
    // none 15
    set_interrupt_system_gate(16, (void *)_x87_FPU_error_wrapper, 0);
    set_interrupt_system_gate(17, (void *)_alignment_check_wrapper, 0);
    set_interrupt_system_gate(18, (void *)_machine_check_wrapper, 0);
    set_interrupt_system_gate(19, (void *)_SIMD_exception_wrapper, 0);
    set_interrupt_system_gate(20, (void *)_virtualization_exception_wrapper, 0);
}

void set_callback(arch::idt::call_func func) { global_call_func = func; }

void set_exit_callback(arch::idt::call_func func) { global_exit_call_func = func; }
} // namespace arch::exception
