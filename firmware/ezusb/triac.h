#ifndef TRIAC_H
#define TRIAC_H

#ifdef DEBUG
typedef int BYTE;
#else
#include <fx2regs.h>
#endif

void init_port_d();
void shift_out_byte(BYTE b);

#endif
