#include "kernel/arch/klib.hpp"
#include "kernel/fs/vfs/file.hpp"
#include "kernel/fs/vfs/vfs.hpp"
#include "kernel/mm/vm.hpp"
#include "kernel/syscall.hpp"
#include "kernel/task.hpp"

namespace syscall
{
bool brk(u64 ptr)
{
    if (!is_user_space_pointer(ptr))
    {
        return false;
    }
    auto info = (memory::vm::info_t *)(task::current_process()->mm_info);
    return info->set_brk(ptr);
}

u64 sbrk(i64 offset)
{
    auto info = (memory::vm::info_t *)(task::current_process()->mm_info);
    if (offset == 0)
    {
        return info->get_brk();
    }
    auto r = info->get_brk();
    info->set_brk(r + offset);
    return r;
}
/// map memory/file
///
/// \param flags 1:read;2:write;4:exec;8:file;16:share
u64 map(u64 map_address, file_desc fd, u64 offset, u64 length, flag_t flags)
{
    if (!is_user_space_pointer(map_address))
    {
        return 0;
    }
    auto &res = task::current_process()->res_table;
    auto vm_info = ((memory::vm::info_t *)(task::current_process()->mm_info));
    fs::vfs::file *file = nullptr;
    if (flags & 8)
    {
        auto file = res.get_file(fd);
        if (!file)
            return -2;
    }
    auto vm = vm_info->map_file(map_address, file, offset, length, flags);

    if (vm)
        return vm->start;
    return 0;

    // ((memory::vm::info_t *)(task::current_process()->mm_info))->mmu_paging.map_area();
}

u64 umap(u64 address)
{
    if (!is_user_space_pointer(address))
    {
        return 0;
    }
    auto &res = task::current_process()->res_table;
    auto vm_info = ((memory::vm::info_t *)(task::current_process()->mm_info));
    vm_info->umap_file(address);
    return 1;
}

BEGIN_SYSCALL

SYSCALL(50, brk)
SYSCALL(51, sbrk)
SYSCALL(52, map)
SYSCALL(53, umap)

END_SYSCALL
} // namespace syscall
