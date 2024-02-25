#ifndef __AETHER_GEOMETRY__
#define __AETHER_GEOMETRY__

#include <math.h>
#include <stdint.h>

#define AE_VEC2_TEMPLATE(type, name)   \
    struct name                        \
    {                                  \
        union                          \
        {                              \
            struct                     \
            {                          \
                type u, v;             \
            };                         \
            struct                     \
            {                          \
                type x, y;             \
            };                         \
            type raw[2];               \
        };                             \
    } name;                            \

#define AE_VEC3_TEMPLATE(type, name)     \
    struct name                          \
    {                                    \
        union                            \
        {                                \
            struct                       \
            {                            \
                type x, y, z;            \
            };                           \
            struct                       \
            {                            \
                type ivert, iuv, inorm;  \
            };                           \
            type raw[3];                 \
        };                               \
    } name;                              \

typedef AE_VEC2_TEMPLATE(double, ae_vec2_f);
typedef AE_VEC2_TEMPLATE(int32_t, ae_vec2_i);
typedef AE_VEC3_TEMPLATE(double, ae_vec3_f);
typedef AE_VEC3_TEMPLATE(int32_t, ae_vec3_i);

#define AE_VEC2_SUM(result, v1, v2) \
    {                               \
        result.u = v1.u + v2.u;     \
        result.v = v1.v + v2.v;     \
    }
#define AE_VEC2_DIF(result, v1, v2) \
    {                               \
        result.u = v1.u - v2.u;     \
        result.v = v1.v - v2.v;     \
    }
#define AE_VEC2_F_MULT(result, v1, f) \
    {                                 \
        result.u = v1.u * f;          \
        result.v = v1.v * f;          \
    }

#define AE_VEC3_CROSS(result, v1, v2)         \
    {                                         \
        result.x = v1.y * v2.z - v1.z * v2.y; \
        result.y = v1.z * v2.x - v1.x * v2.z; \
        result.z = v1.x * v2.y - v1.y * v2.x; \
    }
#define AE_VEC3_SUM(result, v1, v2) \
    {                               \
        result.x = v1.x + v2.x;     \
        result.y = v1.y + v2.y;     \
        result.z = v1.z + v2.z;     \
    }
#define AE_VEC3_DIF(result, v1, v2) \
    {                               \
        result.x = v1.x - v2.x;     \
        result.y = v1.y - v2.y;     \
        result.z = v1.z - v2.z;     \
    }
#define AE_VEC3_F_MULT(result, v1, f) \
    {                                 \
        result.x = v1.x * f;          \
        result.y = v1.y * f;          \
        result.z = v1.z * f;          \
    }
#define AE_VEC3_DOT(v1, v2) v1.x *v2.x + v1.y *v2.y + v1.z *v2.z
#define AE_VEC3_NORM(v1) sqrt(v1.x *v1.x + v1.y * v1.y + v1.z * v1.z)
#define AE_VEC3_NORMALIZE(result, v1, l) \
    {                                    \
        double f = l / AE_VEC3_NORM(v1); \
        AE_VEC3_F_MULT(result, v1, f);   \
    }

#endif //__AETHER_GEOMETRY__
