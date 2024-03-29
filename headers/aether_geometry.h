// based on https://github.com/ssloy/tinyrenderer/wiki
#ifndef __AETHER_GEOMETRY__
#define __AETHER_GEOMETRY__

#include <math.h>
#include <stdint.h>
#include <string.h>

#define AE_VEC2_TEMPLATE(type, name) \
    typedef struct name              \
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
    typedef struct name                 \
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

AE_VEC2_TEMPLATE(double, ae_vec2_f);
AE_VEC2_TEMPLATE(int32_t, ae_vec2_i);
AE_VEC3_TEMPLATE(double, ae_vec3_f);
AE_VEC3_TEMPLATE(int32_t, ae_vec3_i);

#define AE_VEC2_SUM(result, v1, v2) \
    result.u = v1.u + v2.u;         \
    result.v = v1.v + v2.v;

#define AE_VEC2_DIF(result, v1, v2) \
    result.u = v1.u - v2.u;         \
    result.v = v1.v - v2.v;

#define AE_VEC2_F_MULT(result, v1, f) \
    result.u = v1.u * f;              \
    result.v = v1.v * f;

#define AE_VEC2_V_PLUS_DIF_MULT(result, v1, v2, v3, f) \
    result.x = v1.x + (v2.x - v3.x) * f;               \
    result.y = v1.y + (v2.y - v3.y) * f;

#define AE_VEC2_F_CREATE_FROM(result, v) \
    ae_vec2_f result;                    \
    result.x = (double)v.x;              \
    result.y = (double)v.y;
#define AE_VEC2_I_CREATE_FROM(result, v) \
    ae_vec2_i result;                    \
    result.x = (int32_t)v.x;             \
    result.y = (int32_t)v.y;

#define AE_VEC3_CROSS(result, v1, v2)     \
    result.x = v1.y * v2.z - v1.z * v2.y; \
    result.y = v1.z * v2.x - v1.x * v2.z; \
    result.z = v1.x * v2.y - v1.y * v2.x;

#define AE_VEC3_SUM(result, v1, v2) \
    result.x = v1.x + v2.x;         \
    result.y = v1.y + v2.y;         \
    result.z = v1.z + v2.z;

#define AE_VEC3_DIF(result, v1, v2) \
    result.x = v1.x - v2.x;         \
    result.y = v1.y - v2.y;         \
    result.z = v1.z - v2.z;

#define AE_VEC3_F_MULT(result, v1, f) \
    result.x = v1.x * f;              \
    result.y = v1.y * f;              \
    result.z = v1.z * f;

#define AE_VEC3_DOT(v1, v2) v1.x *v2.x + v1.y *v2.y + v1.z *v2.z
#define AE_VEC3_NORM(v1) sqrt(v1.x *v1.x + v1.y * v1.y + v1.z * v1.z)
#define AE_VEC3_NORMALIZE(result, v1, l) \
    {                                    \
        double f = l / AE_VEC3_NORM(v1); \
        AE_VEC3_F_MULT(result, v1, f);   \
    }
#define AE_VEC3_V_PLUS_DIF_MULT(result, v1, v2, v3, f) \
    result.x = v1.x + (v2.x - v3.x) * f;               \
    result.y = v1.y + (v2.y - v3.y) * f;               \
    result.z = v1.z + (v2.z - v3.z) * f;

#define AE_VEC3_F_CREATE_FROM(result, v) \
    ae_vec3_f result;                    \
    result.x = (double)v.x;              \
    result.y = (double)v.y;              \
    result.z = (double)v.z;
#define AE_VEC3_I_CREATE_FROM(result, v) \
    ae_vec3_i result;                    \
    result.x = (int32_t)v.x;             \
    result.y = (int32_t)v.y;             \
    result.z = (int32_t)v.z;

#define AE_MATRIX_F_TEMPLATE(r, c)     \
    typedef struct ae_matrix_##r##_##c \
    {                                  \
        double data[r * c];            \
        size_t rows;                   \
        size_t cols;                   \
    } ae_matrix_##r##_##c;

AE_MATRIX_F_TEMPLATE(3, 3);
AE_MATRIX_F_TEMPLATE(4, 4);
AE_MATRIX_F_TEMPLATE(4, 1);
AE_MATRIX_F_TEMPLATE(1, 4);

#define AE_MATRIX_F_DECLARE(name, r, c) \
    ae_matrix_##r##_##c name

#define AE_MATRIX_F_CREATE(name, r, c) \
    AE_MATRIX_F_DECLARE(name, r, c) = {.rows = r, .cols = c};

#define AE_MATRIX_F_GET(m, r, c) (m.data[(c) + m.cols * (r)])

#define AE_MATRIX_F_IDENTITY(name, dim)                         \
    for (size_t i = 0; i < dim; i++)                            \
    {                                                           \
        for (size_t j = 0; j < dim; j++)                        \
        {                                                       \
            AE_MATRIX_F_GET(name, i, j) = (i == j ? 1.0 : 0.0); \
        }                                                       \
    }

#define AE_MATRIX_F_IDENTITY_CREATE(name, dim) \
    AE_MATRIX_F_CREATE(name, dim, dim);        \
    AE_MATRIX_F_IDENTITY(name, dim);

#define AE_MATRIX_F_MULT(result, m1, m2, r1, c2)                                                        \
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
    }

#define AE_MATRIX_F_MULT_CREATE(result, m1, m2, r1, c2) \
    AE_MATRIX_F_CREATE(result, r1, c2);                 \
    AE_MATRIX_F_MULT(result, m1, m2, r1, c2);

#define AE_MATRIX_F_TRANSPOSE(result, m, r, c)                        \
    for (size_t i = 0; i < m.rows; i++)                               \
    {                                                                 \
        for (size_t j = 0; j < m.cols; j++)                           \
        {                                                             \
            AE_MATRIX_F_GET(result, j, i) = AE_MATRIX_F_GET(m, i, j); \
        }                                                             \
    }

#define AE_MATRIX_F_TRANSPOSE_CREATE(result, m, r, c) \
    AE_MATRIX_F_CREATE(result, r, c);                 \
    AE_MATRIX_F_TRANSPOSE(result, m, r, c);

#define AE_MATRIX_F_INVERSE(result, m, r, c)                                                           \
    {                                                                                                  \
        struct                                                                                         \
        {                                                                                              \
            double data[r * c * 2];                                                                    \
            size_t rows;                                                                               \
            size_t cols;                                                                               \
        } temp = {.rows = r, .cols = c * 2};                                                           \
        for (size_t i = 0; i < m.rows; i++)                                                            \
        {                                                                                              \
            for (size_t j = 0; j < m.cols; j++)                                                        \
            {                                                                                          \
                AE_MATRIX_F_GET(temp, i, j) = AE_MATRIX_F_GET(m, i, j);                                \
                if (i == j)                                                                            \
                    AE_MATRIX_F_GET(temp, i, j + m.cols) = 1.0;                                        \
                else                                                                                   \
                    AE_MATRIX_F_GET(temp, i, j + m.cols) = 0.0;                                        \
            }                                                                                          \
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

#define AE_MATRIX_F_INVERSE_CREATE(result, m, r, c) \
    AE_MATRIX_F_CREATE(result, r, c);               \
    AE_MATRIX_F_INVERSE(result, m, r, c);

#define AE_MATRIX_F_INVERSE_TRANSPOSE(result, m, r, c)                                                 \
    {                                                                                                  \
        struct                                                                                         \
        {                                                                                              \
            double data[r * c * 2];                                                                    \
            size_t rows;                                                                               \
            size_t cols;                                                                               \
        } temp = {.rows = r, .cols = c * 2};                                                           \
        for (size_t i = 0; i < m.rows; i++)                                                            \
        {                                                                                              \
            for (size_t j = 0; j < m.cols; j++)                                                        \
            {                                                                                          \
                AE_MATRIX_F_GET(temp, i, j) = AE_MATRIX_F_GET(m, i, j);                                \
                if (i == j)                                                                            \
                    AE_MATRIX_F_GET(temp, i, j + m.cols) = 1.0;                                        \
                else                                                                                   \
                    AE_MATRIX_F_GET(temp, i, j + m.cols) = 0.0;                                        \
            }                                                                                          \
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
                AE_MATRIX_F_GET(result, j, i) = AE_MATRIX_F_GET(temp, i, j + m.cols);                  \
            }                                                                                          \
        }                                                                                              \
    }

#define AE_MATRIX_F_INVERSE_TRANSPOSE_CREATE(result, m, r, c) \
    AE_MATRIX_F_CREATE(result, r, c);                         \
    AE_MATRIX_F_INVERSE_TRANSPOSE(result, m, r, c);

#define AE_MATRIX_F_SET_ROW_FROM_V_DIF(m, r, v1, v2)  \
    AE_MATRIX_F_GET(m, r, 0) = v1.raw[0] - v2.raw[0]; \
    AE_MATRIX_F_GET(m, r, 1) = v1.raw[1] - v2.raw[1]; \
    AE_MATRIX_F_GET(m, r, 2) = v1.raw[2] - v2.raw[2];

#define AE_MATRIX_F_SET_ROW_FROM_V(m, r, v) \
    AE_MATRIX_F_GET(m, r, 0) = v.raw[0];    \
    AE_MATRIX_F_GET(m, r, 1) = v.raw[1];    \
    AE_MATRIX_F_GET(m, r, 2) = v.raw[2];

#define AE_MATRIX_F_SET_COL_FROM_V(m, c, v) \
    AE_MATRIX_F_GET(m, 0, c) = v.raw[0];    \
    AE_MATRIX_F_GET(m, 1, c) = v.raw[1];    \
    AE_MATRIX_F_GET(m, 2, c) = v.raw[2];

#define AE_MATRIX_3x3_MULT_V(result, m, v)                                                                                               \
    {                                                                                                                                    \
        ae_vec3_f temp = {.x = v.x, .y = v.y, .z = v.z};                                                                                 \
        for (size_t i = 0; i < 3; i++)                                                                                                   \
            result.raw[i] = (AE_MATRIX_F_GET(m, i, 0) * temp.x + AE_MATRIX_F_GET(m, i, 1) * temp.y + AE_MATRIX_F_GET(m, i, 2) * temp.z); \
    }

#endif //__AETHER_GEOMETRY__
