#include <libusb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>

uint16_t vendor_id = 0x0547;
uint16_t product_id = 0xff01;

libusb_context *ctx;
libusb_device_handle* handle;

void set_light(bool illuminate, int light) {
    int transferred;
    int r;
    unsigned char data[2] = {illuminate ? 'I' : 'O', light + '0'};
    r = libusb_bulk_transfer(handle, 0x02, data, 2, &transferred, 0);
    printf("r=%d, transferred=%d\n", r, transferred);
}

int main(int argc, char* argv[]) {
    libusb_init(&ctx);
    libusb_set_debug(ctx, 0);

	handle = libusb_open_device_with_vid_pid(ctx, vendor_id, product_id);
	assert(handle != NULL);

    int config;
    assert(libusb_get_configuration(handle, &config) == 0);
    printf("config=%d\n", config);

    if(config != 1) {
        assert(libusb_set_configuration(handle, 1) == 0);
    }

    assert(libusb_claim_interface(handle, 0) == 0);

    set_light(false, 0);
    set_light(false, 1);
    set_light(false, 2);
    set_light(false, 3);
    set_light(false, 4);
    set_light(false, 5);
    set_light(false, 6);
    set_light(false, 7);
    for(int i = 0; i < 8; i++) {
        set_light(true, i);
        usleep(500 * 1000);
        set_light(false, i);
    }
    for(int i = 7; i >= 0; i--) {
        set_light(true, i);
        usleep(500 * 1000);
        set_light(false, i);
    }
    /*
    set_light(true, 0);
    set_light(true, 1);
    set_light(true, 2);
    set_light(true, 3);
    set_light(true, 4);
    set_light(true, 5);
    set_light(true, 6);
    set_light(true, 7);
    */

    libusb_release_interface(handle, 0);
    libusb_close(handle);
    libusb_exit(ctx);

	return EXIT_SUCCESS;
}
