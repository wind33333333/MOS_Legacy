#ifndef __SMP_INIT__
#define __SMP_INIT__
#include "printk.h"
#include "lib.h"

void ap_init(unsigned int bsp_flags,unsigned int cpu_id);

#endif