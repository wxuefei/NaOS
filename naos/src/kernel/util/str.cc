#include "kernel/util/str.hpp"
namespace util
{
int strcmp(const char *str1, const char *str2)
{
    char s1, s2;
    while (1)
    {
        s1 = *str1++;
        s2 = *str2++;
        if (s1 != s2)
            return s1 < s2 ? -1 : 1;
        if (!s1)
            break;
    }
    return 0;
}
} // namespace util