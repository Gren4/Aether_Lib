#ifndef __AETHER_GARBAGE_COLLECTOR__
#define __AETHER_GARBAGE_COLLECTOR__

#include <stdint.h>

#define _AE_MAIN { init_ae_gc();
#define _AE { on_enter_ae_gc();
#define AE_(x) on_return_ae_gc(); return x;}
#define AE__ on_return_ae_gc(); }

void init_ae_gc(void);
size_t append_ae_gc(void *data);
void update_ae_gc(size_t i, void *data);
void remove_ae_gc(size_t i);

void on_enter_ae_gc(void);
void on_return_ae_gc(void);

#endif //__AETHER_GARBAGE_COLLECTOR__
