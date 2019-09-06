#pragma once
#include "common.hpp"
namespace font
{

class font
{
  public:
    virtual void init() = 0;
    virtual void *get_unicode(u32 code) = 0;
    virtual u8 hit(void *font_data, u32 x, u32 y) = 0;
    virtual void get_size(u32 &width, u32 &height) = 0;
};

} // namespace font
