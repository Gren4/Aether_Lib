#ifndef __AETHER_UTILS__
#define __AETHER_UTILS__

#include <stdint.h>

#define swap_ae(type, a, b) \
    {                       \
        type c = a;         \
        a = b;              \
        b = c;              \
    }

#define min_ae(x, y) ((x) < (y) ? (x) : (y))

#define max_ae(x, y) ((x) > (y) ? (x) : (y))

#define diap_ae(x, from, to) ((x) > (to) ? (to) : ((x) < (from) ? (from) : (x)))

inline size_t find_next_power_of_2(size_t n)
{
    switch(n)
    {
        case 0:
            return 1;
        break;
        case 1:
            return 2;
        break;
        default:
        {
            n--;

            n |= n >> 1;
            n |= n >> 2;
            n |= n >> 4;
            n |= n >> 8;
            n |= n >> 16;
            n |= n >> 32;

            return ++n;
        }
        break;
    }
}

#endif //__AETHER_UTILS__
