#ifndef __AETHER_LOSS_FUNCTIONS__
#define __AETHER_LOSS_FUNCTIONS__

#include "aether_tensor.h"

float MSE_ae_loss(const aether_tensor * const result, const aether_tensor * const reference, aether_tensor * loss);

#endif // __AETHER_LOSS_FUNCTIONS__
