#include "triac-control.h"

int main(int argc, char* argv[]) {
    init_port_d();
    shift_out_byte(0xa5);
}
