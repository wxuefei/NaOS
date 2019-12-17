#pragma once
#include "common.hpp"
#include "kernel/types.hpp"
#include "kernel/util/hash_map.hpp"
#include "kernel/util/id_generator.hpp"

namespace dev
{
class driver;
enum class type
{
    block = 0,
    chr = 1,
    network = 2,
};
using num_t = u32;
constexpr num_t null_num = (-1);
struct device_class;
class device
{
    type dev_type;
    const char *name;
    num_t id;
    driver *current_driver;
    void *user_data;

  public:
    friend int enum_device(device_class *clazz);

    device(type dev_type, const char *name)
        : dev_type(dev_type)
        , name(name)
    {
    }
    num_t get_dev_id() { return id; }

    void *get_user_data() { return user_data; }
    void set_user_data(void *user_data) { this->user_data = user_data; }
    const char *get_name() const { return name; }
    driver *get_driver() { return current_driver; }
    void set_driver(driver *driver) { current_driver = driver; }
};

struct device_class
{
    virtual device *try_scan(int index) = 0;
};

struct dev_hash_func
{
    u64 operator()(num_t num) { return num; }
};

using device_map_t = util::hash_map<num_t, device *, dev_hash_func>;
using device_id_gen_t = util::id_generator;

extern device_map_t *device_map, *unbinding_device_map;

void init();

} // namespace dev
