#include "aether_lib.h"
#include <time.h>

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

size_t MurmurHash2 (const char * key, size_t len)
{
  const size_t m = 0x5bd1e995;
  const size_t seed = 0;
  const int r = 24;

  size_t h = seed ^ len;

  const unsigned char * data = (const unsigned char *)key;
  size_t k = 0;

  while (len >= 4)
  {
      k  = data[0];
      k |= data[1] << 8;
      k |= data[2] << 16;
      k |= data[3] << 24;

      k *= m;
      k ^= k >> r;
      k *= m;

      h *= m;
      h ^= k;

      data += 4;
      len -= 4;
  }

  switch (len)
  {
    case 3:
      h ^= data[2] << 16;
    case 2:
      h ^= data[1] << 8;
    case 1:
      h ^= data[0];
      h *= m;
  };

  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;

  return h;
}

void print_map(ae_map *map)
{

    return;
}

int comp(const void *a, const void *b)
{
    if (*(uint8_t *)a < *(uint8_t *)b)
        return -1;
    else if (*(uint8_t *)a > *(uint8_t *)b)
        return 1;
    else
        return 0;
}

int main(void)
{
    clock_t tic, toc;

    size_t buf = 1;

    ae_vector vec = create_ae_vector(sizeof(size_t), 0);
    tic = clock();
    for (size_t i = 0; i < 1000000; i++)
    {
        append_ae_vector(&vec, &buf);
        buf++;
    }
    toc = clock();
    printf("Execution time: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);
    printf("\n");

    for (size_t i = 0; i < 1000000; i++)
    {
        get_ae_vector(&vec, i, &buf);
        //printf("%lld\n", buf);
    }

    free_ae_vector(&vec);

    tic = clock();
    ae_map map = create_ae_map(sizeof(size_t), NULL);
    toc = clock();
    printf("Execution time: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);
    printf("\n");

    tic = clock();
    char str[80];
    buf = 0;
    size_t key = 0;
    for (size_t i = 0; i < 1000000; i++)
    {
        sprintf(str, "%lld", i);
        set_ae_map(&map, (const char *)&str, &buf);
        buf++;
        key++;
    }

    toc = clock();
    printf("Execution time: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);
    printf("\n");

    sprintf(str, "%lld", 1);
    get_ae_map(&map, (const char *)&str, &buf);
    printf("%lld\n", buf);

    sprintf(str, "%lld", 250);
    get_ae_map(&map, (const char *)&str, &buf);
    printf("%lld\n", buf);

    sprintf(str, "%lld", 500000);
    get_ae_map(&map, (const char *)&str, &buf);
    printf("%lld\n", buf);

    free_ae_map(&map);

    return 0;
}
