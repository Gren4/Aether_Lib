
#ifndef __AETHER_NODE__
#define __AETHER_NODE__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct
{
    void *prev;
    void *next;
    void *data;
} ae_node2w;

typedef struct
{
    void *next;
    void *data;
} ae_node1w;

#endif // __AETHER_NODE__
