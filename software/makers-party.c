#include <assert.h>
#include <libusb.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "makers-party.h"

struct makers_party {
	libusb_context *ctx;
	libusb_device_handle* handle;
	struct light_set_descriptor* light_set;
	struct light_descriptor* lights;
};

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

static void load_descriptors(struct makers_party* makers_party) {
	if(makers_party->light_set != NULL) {
		return;
	}

	struct light_set_descriptor * light_set = malloc(sizeof(struct light_set_descriptor));
	if(light_set == NULL) {
		goto fail;
	}

	// Load the light set descriptor
	int result = libusb_control_transfer(
		makers_party->handle,
		requestType,
		requestGetLightSetDescriptor,
		0,
		0,
		(unsigned char *) light_set,
		light_set_descriptor_size,
		timeout);
	printf("result=%d, light_set_descriptor_size=%d\n", result, light_set_descriptor_size);
	assert(result == light_set_descriptor_size);
//	printf("light_set_descriptor->count=%d\n", light_set->count);

	// Load all the light descriptors
	struct light_descriptor* lights = malloc(sizeof(struct light_descriptor) * light_set->count);
	if(lights == NULL) {
		goto fail;
	}

	result = libusb_control_transfer(
		makers_party->handle,
		requestType,
		requestGetLightDescriptors,
		0, 0,
		(unsigned char *)lights,
		light_descriptor_size * light_set->count,
		timeout);
	printf("result=%d, light_descriptor_size * light_set->count=%d\n", result, (light_descriptor_size * light_set->count));
	assert(result == (light_descriptor_size * light_set->count));

	makers_party->light_set = light_set;
	makers_party->lights = lights;
	return;

fail:
	free(light_set);
	free(lights);
}

bool makers_party_alloc(struct makers_party** makers_party) {
	struct makers_party* mp = malloc(sizeof(struct makers_party));
	if(mp == NULL) {
		goto fail;
	}
	memset(mp, 0, sizeof(struct makers_party));

    if(libusb_init(&mp->ctx)) {
		goto fail;
	}

    libusb_set_debug(mp->ctx, 0);

	mp->handle = libusb_open_device_with_vid_pid(mp->ctx, vendor_id, product_id);
	if(mp->handle == NULL) {
		goto fail;
	}

    int config;
    if(libusb_get_configuration(mp->handle, &config) != 0) {
		goto fail;
	}

    if(config != 1 && libusb_set_configuration(mp->handle, 1) != 0) {
        goto fail;
    }

    if(libusb_claim_interface(mp->handle, 0) != 0) {
		goto fail;
	}

	*makers_party = mp;
	return true;

fail:
	makers_party_dealloc(mp);
	return false;
}

void makers_party_dealloc(struct makers_party* makers_party) {
	if(makers_party == NULL) {
		return;
	}

	free(makers_party->lights);
	free(makers_party->light_set);
	if(makers_party->handle != NULL) {
		libusb_release_interface(makers_party->handle, 0);
		libusb_close(makers_party->handle);
	}
	if(makers_party->ctx != NULL) {
		libusb_exit(makers_party->ctx);
	}
	free(makers_party);
}

void makers_party_set_light(struct makers_party* makers_party, uint8_t light, bool on) {
    int transferred;
    int r;
    unsigned char data[2] = {light, on};
    r = libusb_bulk_transfer(makers_party->handle, 0x02, data, sizeof(data), &transferred, timeout);
    assert(r == 0);
    assert(transferred == sizeof(data));
}

struct light_set_descriptor* makers_get_light_set_descriptor(struct makers_party* makers_party) {
	load_descriptors(makers_party);
	return makers_party->light_set;
}

struct light_descriptor* makers_get_light_descriptors(struct makers_party* makers_party) {
	load_descriptors(makers_party);
	return makers_party->lights;
}
