#include "kernel/arch/video/vga/output_text.hpp"
#include "kernel/util/memory.hpp"

namespace arch::device::vga
{
output_text::output_text(u32 width, u32 height, void *video, u32 bbp, u32 pitch)
    : output(width, height, video, bbp, pitch)
    , allocator(reserved_space)
    , bitmap(&allocator, height)
    , has_write(false)
{
    // height must less than 32 * 8
    bitmap.clean_all();
}

void output_text::init() {}

void output_text::cls()
{
    for (u64 i = 0; i < width * height; i++)
        *((u16 *)video_addr + i) = 0;
}

void output_text::scroll(i32 n)
{
    if (likely(n > 0))
    {
        // move each line
        u64 disp = n * width;
        volatile u16 *fb = (u16 *)video_addr;
        for (u64 i = 0; i < height * width - disp; i++, fb++)
            *fb = *(fb + disp);

        for (u64 i = height * width - disp; i < height * width; i++, fb++)
            *fb = 0; // clean 0
        for (u64 i = 0; i < height; i++)
            bitmap.set(i, 1);
    }
    else if (n < 0)
    {
        // move each line up
        u64 disp = -n * width;
        for (u64 i = height * width - 1; i >= disp; i--)
            *((u16 *)video_addr + i) = *((u16 *)video_addr + i - disp);

        for (u64 i = 0; i < disp; i++)
            *((u16 *)video_addr + i) = 0; // clean 0
    }
    else // n == 0
    {
        return;
    }

    py -= n;
}

void output_text::move_pen(i32 x, i32 y, u32 newline_alignment)
{
    py += y;
    px += x;
    if (y != 0)
        px += newline_alignment;
    if (px >= width)
    {
        px = newline_alignment;
        py++;
    }
    if (py >= height)
        scroll(py - height + 1);
}

u8 similar_color_index(u32 color)
{
    static u32 color_table[] = {0x000000, 0x0000AA, 0x00AA00, 0x00AAAA, 0xAA0000, 0xAA00AA, 0xAA5500, 0xAAAAAA,
                                0x555555, 0x5555FF, 0x55FF55, 0x55FFFF, 0xFF5555, 0xFF55FF, 0xFFFF55, 0xFFFFFF};
    for (int i = 0; i < 16; i++)
    {
        if (color == color_table[i])
            return i;
    }
    // Can not find in color table. Return 0xFFFFFF
    return 15;
}

void output_text::putchar(char ch, font_attribute &attribute)
{
    if (ch != '\t' && ch != '\n')
    {
        u8 fg = similar_color_index(attribute.get_foreground());
        u8 bg = similar_color_index(attribute.get_background());
        *((u8 *)video_addr + (px + py * width) * 2) = ch;
        *((u8 *)video_addr + (px + py * width) * 2 + 1) = fg | (bg << 4);
        bitmap.set(py, 1);
        has_write = true;
        move_pen(1, 0, attribute.get_newline_alignment());
    }
    else
    {
        move_pen(-px, 1, attribute.get_newline_alignment());
    }
}

void output_text::flush(void *vraw)
{
    if (likely(!has_write))
        return;
    u32 line_bytes = width * sizeof(u16);

    for (u32 y = 0; y < height; y++)
    {
        if (bitmap.get(y) != 0)
        {
            util::memcopy((char *)vraw + y * line_bytes, (char *)video_addr + y * line_bytes, line_bytes);
        }
    }
    bitmap.clean_all();
    has_write = false;
}
} // namespace arch::device::vga