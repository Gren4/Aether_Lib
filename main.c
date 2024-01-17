#include "aether_lib.h"
#include <stdio.h>
#include <conio.h>

void print_vec(ae_vector *vec)
{
    uint8_t buf;
    size_t i = 0;
    for (i = 0; i < vec->data.quant; i++)
    {
        f_get_ae_vector(vec, i, &buf);
        printf("%d\n", buf);
    }
    printf("\n");

    return;
}

void print_map(ae_map *map)
{
    double buf;
    uint8_t *key;
    size_t i = 0;

    ae_vector keys = f_get_keys_ae_map(map);

    for (i = 0; i < keys.data.quant; i++)
    {
        f_get_ae_vector(&keys, i, &key);
        f_get_ae_map(map, key, &buf);
        printf("%s is %f\n", key, buf);
    }
    f_free_ae_vector(&keys);
    printf("\n");

    return;
}

void main(void)
{
    uint8_t buf;
    ae_vector vec = f_init_ae_vector();
    f_create_ae_vector(&vec, sizeof(uint8_t));

    buf = 1;
    f_append_ae_vector(&vec, &buf);
    buf = 2;
    f_append_ae_vector(&vec, &buf);
    buf = 3;
    f_append_ae_vector(&vec, &buf);
    buf = 4;
    f_append_ae_vector(&vec, &buf);
    buf = 5;
    f_append_ae_vector(&vec, &buf);

    print_vec(&vec);

    f_invert_ae_vector(&vec);

    print_vec(&vec);

    f_delete_ae_vector(&vec, 1, NULL);

    print_vec(&vec);

    buf = 6;
    f_insert_ae_vector(&vec, 2, &buf);

    print_vec(&vec);

    ae_vector vec2 = f_init_ae_vector();
    f_duplicate_ae_vector(&vec2, &vec);

    print_vec(&vec2);
    print_vec(&vec);

    f_resize_ae_vector(&vec2, 2);

    print_vec(&vec2);

    f_resize_ae_vector(&vec, 10);

    print_vec(&vec);

    buf = 7;
    f_set_ae_vector(&vec, 7, &buf);

    print_vec(&vec);

    f_concat_ae_vector(&vec2, &vec);

    print_vec(&vec2);

    size_t f;
    buf = 2;
    f_find_ae_vector(&vec2, &buf, &f);
    buf = 10;
    f_find_ae_vector(&vec2, &buf, &f);

    f_free_ae_vector(&vec);
    f_free_ae_vector(&vec2);

    ae_map map = f_init_ae_map();
    f_create_ae_map(&map, sizeof(double), NULL);

    double bufd = 1.0;
    f_set_ae_map(&map, "Josh", &bufd);

    if (f_has_key_ae_map(&map, "Josh"))
        printf("Josh is in the list\n");
    else
        printf("Josh isn't in the list\n");

    bufd = 1.5;
    f_set_ae_map(&map, "Anna", &bufd);

    bufd = 2.0;
    f_set_ae_map(&map, "Carl", &bufd);

    bufd = 2.5;
    f_set_ae_map(&map, "Elise", &bufd);

    print_map(&map);

    f_delete_ae_map(&map, "Josh", NULL);

    print_map(&map);

    if (f_has_key_ae_map(&map, "Josh"))
        printf("Josh is in the list\n");
    else
        printf("Josh isn't in the list\n");

    f_free_ae_map(&map);

    return;
}