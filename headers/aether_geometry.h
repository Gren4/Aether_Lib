// based on https://github.com/ssloy/tinyrenderer/wiki
#ifndef __AETHER_GEOMETRY__
#define __AETHER_GEOMETRY__

#include <math.h>
#include <stdint.h>
#include <string.h>

#define AE_VEC2_TEMPLATE(type, name) \
    struct name                      \
    {                                \
        union                        \
        {                            \
            struct                   \
            {                        \
                type u, v;           \
            };                       \
            struct                   \
            {                        \
                type x, y;           \
            };                       \
            type raw[2];             \
        };                           \
    } name;

#define AE_VEC3_TEMPLATE(type, name)    \
    struct name                         \
    {                                   \
        union                           \
        {                               \
            struct                      \
            {                           \
                type x, y, z;           \
            };                          \
            struct                      \
            {                           \
                type ivert, iuv, inorm; \
            };                          \
            type raw[3];                \
        };                              \
    } name;

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

#define AE_MATRIX_F_CREATE(name, r, c) \
    struct                             \
    {                                  \
        double data[r * c];            \
        size_t rows;                   \
        size_t cols;                   \
    } name;                            \
    name.rows = (r);                   \
    name.cols = (c);

#define AE_MATRIX_F_GET(m, r, c) (m.data[(c) + m.cols * (r)])

#define AE_MATRIX_F_IDENTITY(name, dim)                         \
    AE_MATRIX_F_CREATE(name, dim, dim);                         \
    for (size_t i = 0; i < dim; i++)                            \
    {                                                           \
        for (size_t j = 0; j < dim; j++)                        \
        {                                                       \
            AE_MATRIX_F_GET(name, i, j) = (i == j ? 1.0 : 0.0); \
        }                                                       \
    }

#define AE_MATRIX_F_MULT(result, m1, m2, r1, c2)                                                            \
    AE_MATRIX_F_CREATE(result, r1, c2);                                                                     \
    if (m1.cols == m2.rows)                                                                                 \
    {                                                                                                       \
        for (size_t i = 0; i < m1.rows; i++)                                                                \
        {                                                                                                   \
            for (size_t j = 0; j < m2.cols; j++)                                                            \
            {                                                                                               \
                AE_MATRIX_F_GET(result, i, j) = 0.0;                                                        \
                for (size_t k = 0; k < m1.cols; k++)                                                        \
                {                                                                                           \
                    AE_MATRIX_F_GET(result, i, j) += AE_MATRIX_F_GET(m1, i, k) * AE_MATRIX_F_GET(m2, k, j); \
                }                                                                                           \
            }                                                                                               \
        }                                                                                                   \
    }

#define AE_MATRIX_F_TRANSPOSE(result, m, r, c)                        \
    AE_MATRIX_F_CREATE(result, r, c);                                 \
    for (size_t i = 0; i < m.rows; i++)                               \
    {                                                                 \
        for (size_t j = 0; j > m.cols; j++)                           \
        {                                                             \
            AE_MATRIX_F_GET(result, j, i) = AE_MATRIX_F_GET(m, i, j); \
        }                                                             \
    }

#define AE_MATRIX_F_INVERSE(result, m, r, c)                                                           \
    AE_MATRIX_F_CREATE(result, r, c);                                                                  \
    if (m.rows == m.cols)                                                                              \
    {                                                                                                  \
        AE_MATRIX_F_CREATE(temp, r, c * 2);                                                            \
        for (size_t i = 0; i < m.rows; i++)                                                            \
        {                                                                                              \
            for (size_t j = 0; j < m.cols; j++)                                                        \
            {                                                                                          \
                AE_MATRIX_F_GET(temp, i, j) = AE_MATRIX_F_GET(m, i, j);                                \
            }                                                                                          \
            AE_MATRIX_F_GET(temp, i, i + m.cols) = 1.0;                                                \
        }                                                                                              \
                                                                                                       \
        for (size_t i = 0; i < m.rows - 1; i++)                                                        \
        {                                                                                              \
            for (size_t j = temp.cols; j > 0; j--)                                                     \
            {                                                                                          \
                AE_MATRIX_F_GET(temp, i, j - 1) /= AE_MATRIX_F_GET(temp, i, i);                        \
            }                                                                                          \
            for (size_t k = i + 1; k < m.rows; k++)                                                    \
            {                                                                                          \
                double coeff = AE_MATRIX_F_GET(temp, k, i);                                            \
                for (size_t j = 0; j < temp.cols; j++)                                                 \
                {                                                                                      \
                    AE_MATRIX_F_GET(temp, k, j) -= AE_MATRIX_F_GET(temp, i, j) * coeff;                \
                }                                                                                      \
            }                                                                                          \
        }                                                                                              \
                                                                                                       \
        for (size_t j = temp.cols; j >= m.rows; j--)                                                   \
            AE_MATRIX_F_GET(temp, m.rows - 1, j - 1) /= AE_MATRIX_F_GET(temp, m.rows - 1, m.rows - 1); \
                                                                                                       \
        for (size_t i = m.rows - 1; i > 0; i--)                                                        \
        {                                                                                              \
            for (size_t k = i; k > 0; k--)                                                             \
            {                                                                                          \
                double coeff = AE_MATRIX_F_GET(temp, k - 1, i);                                        \
                for (size_t j = 0; j < temp.cols; j++)                                                 \
                {                                                                                      \
                    AE_MATRIX_F_GET(temp, k - 1, j) -= AE_MATRIX_F_GET(temp, i, j) * coeff;            \
                }                                                                                      \
            }                                                                                          \
        }                                                                                              \
        for (size_t i = 0; i < m.rows; i++)                                                            \
        {                                                                                              \
            for (size_t j = 0; j < m.cols; j++)                                                        \
            {                                                                                          \
                AE_MATRIX_F_GET(result, i, j) = AE_MATRIX_F_GET(temp, i, j + m.cols);                  \
            }                                                                                          \
        }                                                                                              \
    }

#endif //__AETHER_GEOMETRY__
