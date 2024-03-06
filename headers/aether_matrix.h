#ifndef __AETHER_MATRIX__
#define __AETHER_MATRIX__

#include <stdint.h>

typedef struct ae_matrix_f
{
    double *data;
    size_t gc_idx;
    size_t rows;
    size_t cols;
} ae_matrix_f;

ae_matrix_f create_ae_matrix_f(size_t rows, size_t cols);

void free_ae_matrix_f(ae_matrix_f *matrix);

ae_matrix_f identity_ae_matrix_f(size_t dim);

double get_ae_matrix_f(ae_matrix_f const *matrix, size_t row, size_t col);

void set_ae_matrix_f(ae_matrix_f *matrix, size_t row, size_t col, const double par);

ae_matrix_f multiply_by_matrix_ae_matrix_f(ae_matrix_f const *matrix1, ae_matrix_f const *matrix2);

ae_matrix_f transpose_ae_matrix_f(ae_matrix_f const *matrix);

ae_matrix_f invert_ae_matrix_f(ae_matrix_f const *matrix);

#endif //__AETHER_MATRIX__
