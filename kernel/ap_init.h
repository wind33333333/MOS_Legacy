#ifndef __SMP_INIT__
#define __SMP_INIT__
#include "printk.h"
#include "lib.h"
#include "cpuinfo.h"

void ap_init(unsigned int cpu_id);

#endif