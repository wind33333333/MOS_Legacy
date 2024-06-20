#include "ignore.h"

void do_ignore(void) {

    color_printk(RED,BLACK,"Unknown interrupt or fault at RIP\n");
    while (1);
    return;
}