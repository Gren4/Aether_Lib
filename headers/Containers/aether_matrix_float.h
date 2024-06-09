#ifndef __AETHER_MATRIX__
#define __AETHER_MATRIX__

#include <stdint.h>

#define INIT_AE_MATRIX                                  \
    {                                                   \
        .data = NULL, .gc_idx = 0, .rows = 0, .cols = 0 \
    }

typedef struct ae_mat_f
{
    double *data;
    int32_t gc_idx;
    uint32_t rows;
    uint32_t cols;
} ae_mat_f;

typedef union ae_vec3_f
{
    struct ord
    {
        double x;
        double y;
        double z;
    };
    double data[3];
} ae_vec3_f;

ae_mat_f create_ae_mat_f(const uint32_t rows, const uint32_t cols);

ae_mat_f identity_ae_mat_f(const uint32_t dim);

inline uint32_t rows_ae_mat_f(const ae_mat_f *const  matrix)
{
    return matrix->rows;
}

inline uint32_t cols_ae_mat_f(const ae_mat_f *const  matrix)
{
    return matrix->cols;
}

void free_ae_mat_f(ae_mat_f *matrix);

double *get_ae_mat_f(ae_mat_f const *matrix, const uint32_t row, const uint32_t col);

ae_mat_f resize_temp_ae_mat_f(ae_mat_f *const m_matrix, const ae_mat_f *const matrix, const uint32_t rows, const uint32_t cols);

ae_mat_f sum_ae_mat_f(ae_mat_f *const m_matrix, const ae_mat_f *const matrix1, const ae_mat_f *const matrix2);

ae_mat_f sub_ae_mat_f(ae_mat_f *const m_matrix, const ae_mat_f *const matrix1, const ae_mat_f *const matrix2);

ae_mat_f mult_by_sclr_ae_mat_f(ae_mat_f *const m_matrix, const ae_mat_f *const matrix, const double scalar);

ae_mat_f mult_by_mat_ae_mat_f(ae_mat_f *const m_matrix, const ae_mat_f *const matrix1, const ae_mat_f *const matrix2);

ae_mat_f transpose_ae_mat_f(ae_mat_f *const m_matrix, const ae_mat_f *const matrix);

ae_mat_f invert_ae_mat_f(ae_mat_f *const m_matrix, const ae_mat_f *const matrix);

ae_mat_f invert_transpose_ae_mat_f(ae_mat_f *const m_matrix, const ae_mat_f *const matrix);

double det_ae_mat_f(const ae_mat_f *const matrix);

ae_mat_f adjugate_ae_mat_f(ae_mat_f *const m_matrix, const ae_mat_f *const matrix);

ae_mat_f v3_cross_ae_mat_f(ae_mat_f *const m_matrix, const ae_mat_f *const matrix1, const ae_mat_f *const matrix2);

double v3_dot_ae_mat_f(const ae_mat_f *const matrix1, const ae_mat_f *const matrix2);

double v3_norm_ae_mat_f(const ae_mat_f *const matrix);

ae_mat_f v3_normalize_ae_mat_f(ae_mat_f *const m_matrix, const ae_mat_f *const matrix);

#endif //__AETHER_MATRIX__
