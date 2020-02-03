#include "kernel/fs/vfs/pseudo.hpp"
#include "kernel/fs/vfs/defines.hpp"
namespace fs::vfs
{

bool pipe_write_func(u64 data)
{
    auto *pipe = (pseudo_pipe_t *)data;
    return pipe->is_close || !pipe->buffer.is_full();
}

bool pipe_read_func(u64 data)
{
    auto *pipe = (pseudo_pipe_t *)data;
    return pipe->is_close || pipe->buffer.data_size() != 0;
}

u64 pseudo_pipe_t::write(const byte *data, u64 size, flag_t flags)
{
    for (u64 i = 0; i < size; i++)
    {
        if (buffer.is_full() && !(flags & rw_flags::override))
        {
            if (i > 0)
                return i;
            if (is_close)
                return -1;
            if (flags & rw_flags::no_block)
                return -1;
            task::do_wake_up(&wait_queue);
            task::do_wait(&wait_queue, pipe_write_func, (u64)this, task::wait_context_type::interruptable);
        }
        buffer.write_with() = data[i];
    }
    task::do_wake_up(&wait_queue);
    return size;
}

u64 pseudo_pipe_t::read(byte *data, u64 max_size, flag_t flags)
{
    for (u64 i = 0; i < max_size; i++)
    {
        if (buffer.data_size() == 0)
        {
            if (i > 0)
                return i;
            if (is_close)
                return -1;
            if (flags & rw_flags::no_block)
                return -1;
            task::do_wake_up(&wait_queue);
            task::do_wait(&wait_queue, pipe_read_func, (u64)this, task::wait_context_type::interruptable);
        }
        buffer.read(&data[i]);
    }
    task::do_wake_up(&wait_queue);
    return max_size;
}

void pseudo_pipe_t::close()
{
    is_close = true;
    task::do_wake_up(&wait_queue);
}

} // namespace fs::vfs