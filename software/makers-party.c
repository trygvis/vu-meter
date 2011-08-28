#include <assert.h>
#include <libusb.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "makers-party.h"

static const uint16_t vendor_id = 0x0547;
static const uint16_t product_id = 0xff01;

// Constants for the USB communication.
static const unsigned char endpoint = 0x02;
static const uint8_t requestType = 0xc0;
static const uint8_t requestGetLightSetDescriptor = 0x21;
static const uint8_t requestGetLightDescriptors = 0x22;
static const int timeout = 1000;

static const int light_set_descriptor_size = 1;
static const int light_descriptor_size = 1;

static libusb_context *ctx;
static libusb_device_handle* handle;

struct light_set_descriptor* light_set_descriptor = NULL;

void makers_party_alloc() {
    libusb_init(&ctx);
    libusb_set_debug(ctx, 0);

	handle = libusb_open_device_with_vid_pid(ctx, vendor_id, product_id);
	assert(handle != NULL);

    int config;
    assert(libusb_get_configuration(handle, &config) == 0);

    if(config != 1) {
        assert(libusb_set_configuration(handle, 1) == 0);
    }

    assert(libusb_claim_interface(handle, 0) == 0);
}

void makers_party_dealloc() {
	if(light_set_descriptor != NULL) {
		free(light_set_descriptor->descriptors);
	}
	free(light_set_descriptor);
    libusb_release_interface(handle, 0);
    libusb_close(handle);
    libusb_exit(ctx);
}

void makers_party_set_light(uint8_t light, bool on) {
    int transferred;
    int r;
    unsigned char data[2] = {light, on};
    r = libusb_bulk_transfer(handle, 0x02, data, sizeof(data), &transferred, timeout);
    assert(r == 0);
    assert(transferred == sizeof(data));
}

struct light_set_descriptor* makers_get_light_set_descriptor() {
	int result;

	if(light_set_descriptor != NULL) {
		return light_set_descriptor;
	}

	assert((light_set_descriptor = malloc(sizeof(struct light_set_descriptor))) != NULL);
	// Load the light set descriptor
	result = libusb_control_transfer(
		handle,
		requestType,
		requestGetLightSetDescriptor,
		0,
		0,
		(unsigned char *) light_set_descriptor,
		light_set_descriptor_size,
		timeout);
	printf("light_set_descriptor=%d\n", result);
	assert(result == light_set_descriptor_size);
	printf("light_set_descriptor->count=%d\n", light_set_descriptor->count);

	// Load all the light descriptors
	assert((light_set_descriptor->descriptors = malloc(sizeof(struct light_descriptor) * light_set_descriptor->count)) != NULL);

	result = libusb_control_transfer(
		handle,
		requestType,
		requestGetLightDescriptors,
		0, 0,
		(unsigned char *)light_set_descriptor->descriptors,
		light_descriptor_size * light_set_descriptor->count,
		timeout);
	printf("result=%d\n", result);
	printf("result 2=%d\n", (light_descriptor_size * light_set_descriptor->count));
	assert(result == (light_descriptor_size * light_set_descriptor->count));

	return light_set_descriptor;
}
