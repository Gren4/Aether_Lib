#ifndef __AETHER_GARBAGE_COLLECTOR__
#define __AETHER_GARBAGE_COLLECTOR__

#include <stdint.h>

#define _AE_START_MAIN { init_ae_gc();
#define _AE_START { on_enter_ae_gc();
#define AE_END_(x) on_return_ae_gc(); return x;}
#define AE_END__ on_return_ae_gc(); }

void init_ae_gc(void);
int32_t append_ae_gc(void *data);
void update_ae_gc(size_t i, void *data);
void remove_ae_gc(size_t i);

void on_enter_ae_gc(void);
void on_return_ae_gc(void);

#endif //__AETHER_GARBAGE_COLLECTOR__
