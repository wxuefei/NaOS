#pragma once
#include "../../types.hpp"
#include "common.hpp"
namespace fs
{
namespace permission_flags
{
enum permission : flag_t
{
    exec = 1,
    write = 2,
    read = 4,

    group_exec = 16,
    group_write = 32,
    group_read = 64,

    other_exec = 256,
    other_write = 512,
    other_read = 1024,
};
} // namespace permission_flags

namespace access_flags
{
enum access_flags : flag_t
{
    exec = 1,
    write = 2,
    read = 4,
    exist = 8,
};
} // namespace access_flags

namespace mode
{
// open file mode
enum mode : flag_t
{
    read = 1,
    write = 2,
    bin = 4,
    append = 8,
};
} // namespace mode

namespace path_walk_flags
{
enum path_walk_flags : flag_t
{
    auto_create_file = 1,
    auto_create_dir_rescure = 2,
    parent = 4,
    not_symlink = 8,
    directory = 16,
    file = 32,
    cross_root = 64,
};
} // namespace path_walk_flags

enum class inode_type_t : u8
{
    file,
    directory,
    link,
    device,
    other,
};
namespace vfs
{
class inode;
class dentry;
class file;
class file_system;
struct nameidata;
class super_block;
} // namespace vfs

struct dirstream
{
    file_desc fd;
};

namespace fs_flags
{
enum : flag_t
{
    kernel_only = 1,

};
} // namespace fs_flags

} // namespace fs
