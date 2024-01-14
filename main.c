#include "aether_lib.h"
#include <stdio.h>
#include <conio.h>

void main(void)
{
    ae_map *map = create_ae_map(uint8_t);
    set_ae_map(map, uint8_t, "Anna", 18);
    set_ae_map(map, uint8_t, "Josh", 16);
    set_ae_map(map, uint8_t, "Carl", 22);
    set_ae_map(map, uint8_t, "Elise", 25);
    delete_ae_map(map, uint8_t, "Josh");
    set_ae_map(map, uint8_t, "Leon", 45);

    ae_vector *keys = get_keys_ae_map(map);
    append_ae_vector(keys, uint8_t *, "George");

    size_t i = 0;
    for (i = 0; i < size_ae_vector(keys); i++)
    {
        uint8_t *key = get_ae_vector(keys, i, uint8_t *);
        if (has_key_ae_map(map, key))
            printf("%s is %d years old\n", key, get_ae_map(map, uint8_t, key));
        else
            printf("%s is not in the list\n", key);
    }

    free_ae_map(map);
    free_ae_vector(keys);

    ae_vector *test = create_ae_vector(uint8_t);

    append_ae_vector(test, uint8_t, 2);
    append_ae_vector(test, uint8_t, 5);
    append_ae_vector(test, uint8_t, 8);
    append_ae_vector(test, uint8_t, 1);

    size_t element = 0;
    find_ae_vector(element, test, uint8_t, 5);
    printf("%d\n", element);

    return;
}