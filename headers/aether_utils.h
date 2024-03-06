#ifndef __AETHER_UTILS__
#define __AETHER_UTILS__

#define swap_ae(type, a, b) \
    {                       \
        type c = a;         \
        a = b;              \
        b = c;              \
    }

#define min_ae(x, y) ((x) < (y) ? (x) : (y));

#define max_ae(x, y) ((x) > (y) ? (x) : (y));

#define diap_ae(x, from, to) ((x) > (to) ? (to) : ((x) < (from) ? (from) : (x)))

#endif //__AETHER_UTILS__
