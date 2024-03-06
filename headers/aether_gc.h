#ifndef __AETHER_GARBAGE_COLLECTOR__
#define __AETHER_GARBAGE_COLLECTOR__

#include <stdint.h>

struct ae_garbage_;
typedef struct ae_garbage_ ae_garbage;

void init_ae_gc(void);
size_t append_ae_gc(void *data);
void update_ae_gc(size_t i, void *data);
void remove_ae_gc(size_t i);

void on_enter_ae_gc(void);
void on_return_ae_gc(void);


#endif //__AETHER_GARBAGE_COLLECTOR__
