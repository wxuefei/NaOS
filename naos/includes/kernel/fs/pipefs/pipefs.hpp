#pragma once
#include "../../util/hash_map.hpp"
#include "../vfs/dentry.hpp"
#include "../vfs/file.hpp"
#include "../vfs/file_system.hpp"
#include "../vfs/inode.hpp"
#include "../vfs/super_block.hpp"
#include "common.hpp"
namespace fs::pipefs
{
void init();
class super_block;
class inode;
class dentry : public vfs::dentry
{
  public:
    friend class super_block;
    using vfs::dentry::dentry;
};

class inode : public vfs::inode
{
    friend class super_block;
    friend class file;

  public:
    bool create_symbolink(vfs::dentry *entry, const char *target) override;
    const char *symbolink() override;
};

class file : public vfs::file
{
  public:
    void flush() override{};

  protected:
    u64 iwrite(const byte *buffer, u64 size, flag_t flags) override;
    u64 iread(byte *buffer, u64 max_size, flag_t flags) override;
};

class file_system : public vfs::file_system
{
  public:
    file_system(const char *fsname = "pipefs");
    virtual vfs::super_block *load(const char *device_name, const byte *data, u64 size) override;
    void unload(vfs::super_block *block) override;
};

struct member_hash
{
    u64 operator()(u64 i) { return i; }
};

class super_block : public vfs::super_block
{
  private:
    friend class file_system;
    util::hash_map<u64, inode *, member_hash> inode_map;
    int last_inode_index;

  public:
    super_block(file_system *fs);
    void load() override;
    void save() override;
    void dirty_inode(vfs::inode *node) override;

    void fill_dentry(vfs::dentry *entry) override;
    void save_dentry(vfs::dentry *entry) override;

    void write_inode(vfs::inode *node) override;
    vfs::inode *get_inode(u64 node_index);

    file *alloc_file() override;
    void dealloc_file(vfs::file *f) override;
    inode *alloc_inode() override;
    void dealloc_inode(vfs::inode *node) override;
    dentry *alloc_dentry() override;
    void dealloc_dentry(vfs::dentry *entry) override;
};

void init();
} // namespace fs::pipefs
