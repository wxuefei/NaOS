#include "kernel/arch/arch.hpp"
#include "kernel/arch/cpu.hpp"
#include "kernel/arch/gdt.hpp"
#include "kernel/arch/idt.hpp"
#include "kernel/arch/paging.hpp"
#include "kernel/arch/tss.hpp"
#include "kernel/arch/vbe.hpp"
#include "kernel/mm/memory.hpp"
namespace arch
{
ExportC Unpaged_Text_Section void temp_init(const kernel_start_args *args)
{
    gdt::init_before_paging();
    paging::temp_init();
}
void init(const kernel_start_args *args)
{
    device::vbe::init();
    if (sizeof(kernel_start_args) != args->size_of_struct)
    {
        trace::panic("kernel args is invalid.");
    }

    trace::debug("Kernel starting...");
    cpu::init();

    trace::debug("Memory init...");
    memory::init(args, 0x0);

    trace::debug("Paging init...");
    paging::init();
    device::vbe::mm_addr();

    trace::debug("GDT init...");
    gdt::init_after_paging();

    trace::debug("TSS init...");
    tss::init((void *)0x9fff, (void *)0x6000);

    trace::debug("IDT init...");
    idt::init_after_paging();
}

} // namespace arch
