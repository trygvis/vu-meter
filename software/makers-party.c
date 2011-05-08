#include <assert.h>
#include <libusb.h>
#include <stdbool.h>
#include <stdio.h>
/*
#include <stdlib.h>
#include <unistd.h>
*/

static uint16_t vendor_id = 0x0547;
static uint16_t product_id = 0xff01;

static libusb_context *ctx;
static libusb_device_handle* handle;

void makers_party_alloc() {
    libusb_init(&ctx);
    libusb_set_debug(ctx, 0);

	handle = libusb_open_device_with_vid_pid(ctx, vendor_id, product_id);
	assert(handle != NULL);

    int config;
    assert(libusb_get_configuration(handle, &config) == 0);
    //printf("config=%d\n", config);

    if(config != 1) {
        assert(libusb_set_configuration(handle, 1) == 0);
    }

    assert(libusb_claim_interface(handle, 0) == 0);
}

void makers_party_dealloc() {
    libusb_release_interface(handle, 0);
    libusb_close(handle);
    libusb_exit(ctx);
}

void makers_party_set_light(int light, bool on) {
    int transferred;
    int r;
    unsigned char data[2] = {on ? 'I' : 'O', light + '0'};
    r = libusb_bulk_transfer(handle, 0x02, data, 2, &transferred, 0);
    printf("r=%d, transferred=%d\n", r, transferred);
}
