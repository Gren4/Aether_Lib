#include "aether_lib.h"

void print_vec(ae_vector *vec)
{
    uint8_t buf;
    size_t i = 0;
    for (i = 0; i < vec->data.quant; i++)
    {
        get_ae_vector(vec, i, &buf);
        printf("%d ", buf);
    }
    printf("\n");

    return;
}

void print_map(ae_map *map)
{
    double buf;
    const char *key;
    size_t i = 0;

    ae_vector keys = get_keys_ae_map(map);

    for (i = 0; i < keys.data.quant; i++)
    {
        get_ae_vector(&keys, i, &key);
        get_ae_map(map, key, &buf);
        printf("%s is %f\n", key, buf);
    }
    free_ae_vector(&keys);
    printf("\n");

    return;
}

int comp(const void *a, const void *b)
{
    if (*(uint8_t*)a < *(uint8_t*)b)
        return -1;
    else if (*(uint8_t*)a > *(uint8_t*)b)
        return 1;
    else
        return 0;
}

int main(void)
{
    uint8_t buf;
    ae_vector vec = init_ae_vector();
    create_ae_vector(&vec, sizeof(uint8_t));

    buf = 1;
    append_ae_vector(&vec, &buf);
    buf = 2;
    append_ae_vector(&vec, &buf);
    buf = 3;
    append_ae_vector(&vec, &buf);
    buf = 4;
    append_ae_vector(&vec, &buf);
    buf = 5;
    append_ae_vector(&vec, &buf);

    print_vec(&vec);

    invert_ae_vector(&vec);

    print_vec(&vec);

    delete_ae_vector(&vec, 1, NULL);

    print_vec(&vec);

    buf = 6;
    insert_ae_vector(&vec, 2, &buf);

    print_vec(&vec);

    ae_vector vec2 = init_ae_vector();
    duplicate_ae_vector(&vec2, &vec);

    print_vec(&vec2);

    resize_ae_vector(&vec2, 2);

    print_vec(&vec2);

    resize_ae_vector(&vec, 10);

    print_vec(&vec);

    buf = 7;
    set_ae_vector(&vec, 7, &buf);

    print_vec(&vec);

    concat_ae_vector(&vec2, &vec);

    sort_ae_vector(&vec2, &comp);

    print_vec(&vec2);

    size_t f;
    buf = 2;
    find_ae_vector(&vec2, &buf, &f);
    buf = 10;
    find_ae_vector(&vec2, &buf, &f);

    free_ae_vector(&vec);
    free_ae_vector(&vec2);

    ae_map map = init_ae_map();
    create_ae_map(&map, sizeof(double), NULL);

    double bufd = 1.0;
    set_ae_map(&map, "Josh", &bufd);

    if (has_key_ae_map(&map, "Josh"))
        printf("Josh is in the list\n");
    else
        printf("Josh isn't in the list\n");

    bufd = 1.5;
    set_ae_map(&map, "Anna", &bufd);

    bufd = 2.0;
    set_ae_map(&map, "Carl", &bufd);

    bufd = 2.5;
    set_ae_map(&map, "Elise", &bufd);

    print_map(&map);

    delete_ae_map(&map, "Josh", NULL);

    print_map(&map);

    if (has_key_ae_map(&map, "Josh"))
        printf("Josh is in the list\n");
    else
        printf("Josh isn't in the list\n");

    free_ae_map(&map);

    return 0;
}
