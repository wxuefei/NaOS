#pragma once
#include "common.hpp"
#include "defines.hpp"
namespace fs::vfs
{
extern super_block *pipe_block;
extern dentry *global_root;

void init();
int register_fs(file_system *fs);
int unregister_fs(file_system *fs);
file_system *get_file_system(const char *name);

dentry *path_walk(const char *name, dentry *root, dentry *cur_dir, flag_t flags, nameidata &idata);
dentry *path_walk(const char *name, dentry *root, dentry *cur_dir, flag_t flags);

file *open(const char *path, dentry *root, dentry *cur_dir, flag_t mode, flag_t attr);

bool create(const char *path, dentry *root, dentry *cur_dir, flag_t flags);

bool mkdir(const char *dir, dentry *root, dentry *cur_dir, flag_t attr);
bool rmdir(const char *dir, dentry *root, dentry *cur_dir);

bool rename(const char *new_dir, const char *old_dir, dentry *root, dentry *cur_dir);

u64 pathname(dentry *root, dentry *current, char *path, u64 max_len);
bool access(const char *pathname, dentry *path_root, dentry *cur_dir, flag_t flags);

bool link(const char *src, const char *target, dentry *root, dentry *cur_dir);
bool unlink(dentry *target);
bool unlink(const char *pathname, dentry *root, dentry *cur_dir);
bool symbolink(const char *src, const char *target, dentry *root, dentry *cur_dir, flag_t flags);

bool mount(file_system *fs, const char *dev, const char *path, dentry *path_root, dentry *cur_dir, const byte *data,
           u64 max_len);
bool umount(const char *path, dentry *path_root, dentry *cur_dir);

bool fcntl(file *f, u64 operator_type, u64 target, u64 attr, u64 *value, u64 size);

u64 size(file *f);

file *open_pipe();
file *create_fifo(const char *path, dentry *root, dentry *current, flag_t mode);

} // namespace fs::vfs
