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

    ae_vector * keys = get_keys_ae_map(map);

    size_t i = 0;
    for (i = 0; i < size_ae_vector(keys); i++)
    {
        uint8_t* key = get_ae_vector(keys, i, uint8_t*);
        printf("%s is %d years old\n", key, get_ae_map(map, uint8_t, key));
    }
    
    free_ae_map(map);
    free_ae_vector(keys);

    return;
}