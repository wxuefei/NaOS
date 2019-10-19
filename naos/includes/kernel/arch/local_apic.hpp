#pragma once
#include "common.hpp"

#include "kernel/clock/clock_event.hpp"
#include "kernel/clock/clock_source.hpp"
#include "kernel/irq.hpp"
namespace arch::idt
{
struct regs_t;
} // namespace arch::idt

namespace arch::APIC
{
namespace lvt_index
{
enum lvt_index : u8
{
    cmci,
    timer,
    thermal,
    performance,
    lint0,
    lint1,
    error,
};
}

void local_init();
void local_software_enable();
void local_software_disable();
void local_enable(u8 vector);
void local_irq_setup(u8 index, u8 vector, u8 flags);
void local_disable(u8 vector);
void local_EOI(u8 index);
void local_post_PIP();
class clock_source;

class clock_event : public ::clock::clock_event
{
  private:
    friend irq::request_result on_event(const arch::idt::regs_t *regs, u64 extra_data, u64 user_data);
    friend class clock_source;
    volatile bool is_suspend;
    volatile u64 tick_count;
    volatile u32 init_counter;
    volatile u32 divide;
    volatile u64 bus_frequency;

    u64 hz;

  public:
    clock_event()
        : ::clock::clock_event(90)
    {
    }
    void init(u64 HZ) override;
    void destroy() override;

    void suspend() override;
    void resume() override;

    void wait_next_tick() override;

    bool is_valid() override { return true; }
};

class clock_source : public ::clock::clock_source
{
  private:
    friend class clock_event;

  public:
    void init() override;
    void destroy() override;
    u64 current() override;

    void calibrate(::clock::clock_source *cs) override;
    u64 calibrate_apic(::clock::clock_source *cs);
};

clock_source *make_clock();
} // namespace arch::APIC