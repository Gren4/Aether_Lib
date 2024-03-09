#include "aether_base.h"

extern inline size_t find_next_power_of_2(size_t n);

extern inline void check_realloc_ae_base(ae_base *const base, const size_t *const data_size, size_t new_size, AE_BASE_MEM type);

extern inline void create_gc_ae_base(ae_base *const base, const size_t *const data_size);

extern inline ae_base create_ae_base(const size_t *const data_size, size_t new_size);

extern inline ae_base create_max_size_ae_base(const size_t *const data_size, size_t new_size);

extern inline size_t gc_idx_ae_base(ae_base *const base);

extern inline size_t quant_ae_base(ae_base *const base);

extern inline size_t max_quant_ae_base(ae_base *const base);

extern inline void free_ae_base(ae_base *const base);

extern inline void resize_ae_base(ae_base *const base, const size_t *const data_size, size_t new_size);

extern inline void *append_ae_base(ae_base *const base, const size_t *const data_size);

extern inline void append_base_ae_base(ae_base *const base_to, ae_base *const base_from, const size_t *const data_size, size_t i, size_t n);

extern inline void set_base_ae_base(ae_base *const base_to, ae_base *const base_from, const size_t *const data_size, size_t i_to, size_t i_from, size_t n);

extern inline void *get_ae_base(const ae_base *const base, const size_t *const data_size, size_t i);

extern inline ae_base concat_ae_base(ae_base *c_base, ae_base *const base_to, ae_base *const base_from, const size_t *const data_size);

extern inline void *insert_ae_base(ae_base *const base, const size_t *const data_size, size_t i);

extern inline void insert_base_ae_base(ae_base *const base_to, ae_base *const base_from, const size_t *const data_size, size_t i_to, size_t i_from, size_t n);

extern inline void delete_ae_base(ae_base *const base, const size_t *const data_size, size_t i, void *const par);

extern inline void delete_base_ae_base(ae_base *const base, const size_t *const data_size, size_t i, size_t n, ae_base *par);

extern inline void pop_ae_base(ae_base *const base, const size_t *const data_size, void *const par);

extern inline void pop_base_ae_base(ae_base *const base, const size_t *const data_size, size_t n, ae_base *par);

extern inline ae_base duplicate_ae_base(ae_base *d_base, const ae_base *const base, const size_t *const data_size);

extern inline void invert_ae_base(ae_base *const base, const size_t *const data_size);

extern inline size_t find_ae_base(const ae_base *const base, const size_t *const data_size, void *const par);

extern inline void sort_ae_base(ae_base *const base, const size_t *const data_size, int (*comparator)(const void *, const void *));

extern inline void swap_ae_base(ae_base *const base, const size_t *const data_size, size_t i, size_t j);

extern inline void optimize_ae_base(ae_base *const base, const size_t *const data_size);
