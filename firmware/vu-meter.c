#include <fx2regs.h>
#include <fx2macros.h>
#include <fx2ints.h>
#include <fx2extra.h>
#include <eputils.h>
#include <delay.h>
#include <autovector.h>
#include <setupdat.h>

#include "triac-control.h"

#define SYNCDELAY() SYNCDELAY4;

volatile __bit dosuspend=FALSE;
volatile __bit got_sud;
volatile WORD counter;

void init_usb() {
    REVCTL=0; // not using advanced endpoint controls

    got_sud=FALSE;
    RENUMERATE_UNCOND();

    SETCPUFREQ(CLK_48M);
    SETCPUFREQ(CLK_12M);

    SETIF48MHZ();

    USE_USB_INTS();

    ENABLE_SUDAV();
    ENABLE_USBRESET();
    ENABLE_HISPEED();

    // Endpoint 1
    EP1INCFG &= ~bmVALID;
    SYNCDELAY();

    // Endpoint 2
    // valid=1, direction=0 out, type=10 bulk, size=0 (512), reserved=0, buffering=10 (double)
//    EP2CFG = 0xA2; // 10100010
    // The device refuses to enumerate with buffering=triple or quad
    EP2CFG = bmVALID + EPCFG_DIRECTION_OUT + EPCFG_TYPE_BULK + EPCFG_BUFFER_DOUBLE;
    SYNCDELAY();

    // Endpoint 4
    EP4CFG &= ~bmVALID;
    SYNCDELAY();

    // Endpoint 6
    // valid=1, direction=1 in, type=10 bulk, size=0 (512), reserved=0, buffering=10 (double)
    // EP6CFG = 0xE2; // 11100010
    SYNCDELAY();

    // Endpoint 8
    // EP2CFG = bmVALID + EPCFG_DIRECTION_IN + EPCFG_TYPE_INT + EPCFG_BUFFER_DOUBLE;
    SYNCDELAY();
}

void init_port_a() {
    PORTACFG=0x00;      // port A = IO
    OEA = 0x00;         // port A[0:7] = in
}

void init_port_b() {
    IOB = 0x00;
    OEB = 0xff;         // port B[0:7] = out
}

void main(void)
{
    volatile WORD count = 0;
    volatile BYTE last_ioa = 0;
    volatile BYTE current_ioa = 0;

    init_usb();
    init_port_a();
    init_port_b();
    init_port_d();

    // Arm EP2 to tell the host that we're ready to receive
    EP2BCL = 0x80;
    SYNCDELAY();
    EP2BCL = 0x80;
    SYNCDELAY();

    EA=1;

    IOB = 0xff;

    // loop endlessly
    while(1) {
        if (got_sud) {
            handle_setupdata();
            got_sud = FALSE;
        }

        // All EP2 buffers are empty, nothing to do
        if(EP2468STAT & bmEP2EMPTY) {
            continue;
        }

        count = MAKEWORD(EP2BCH, EP2BCL);

        if(count == 1) {
            shift_out_byte(EP2FIFOBUF[0]);
        }

        EP2BCL=0x80; // Arms EP2
        SYNCDELAY();
    }
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------

/*
BOOL handle_vendorcommand(BYTE cmd) {
    cmd = cmd;
    return FALSE;
}
*/

BOOL handle_vendorcommand(BYTE bRequest) {
    BYTE bmRequestType = SETUPDAT[0];
    struct light_set_descriptor* light_set_descriptor;
    struct light_descriptor* light_descriptors;

    SUDPTRCTL = 1;

    shift_out_byte(0x00);
    if(bmRequestType == 0xc0 && bRequest == 0x21) {
        light_set_descriptor = triac_get_light_set_descriptor();
        EP0BCH = 0;
        EP0BCL = sizeof(struct light_set_descriptor);
        SUDPTRCTL = 0;
        SUDPTRH = (BYTE)((((unsigned short)light_set_descriptor) >> 8) & 0xff);
        SUDPTRL = (BYTE)(((unsigned short)light_set_descriptor) & 0xff);
        shift_out_byte(0xf0);
    }
    else if(bmRequestType == 0xc0 && bRequest == 0x22) {
        light_set_descriptor = triac_get_light_set_descriptor();
        light_descriptors = triac_get_light_descriptors();
        EP0BCH = 0;
        EP0BCL = sizeof(struct light_descriptor) * light_set_descriptor->count;
        SUDPTRCTL = 0;
        SUDPTRH = (BYTE)((((unsigned short)light_descriptors) >> 8) & 0xff);
        SUDPTRL = (BYTE)(((unsigned short)light_descriptors) & 0xff);
        shift_out_byte(0xf0);
    }
    else {
        shift_out_byte(0x0f);
        return FALSE;
    }

    EP0CS |= bmHSNAK;
    return TRUE;
}

BOOL handle_get_interface(BYTE ifc, BYTE* alt_ifc) {
    if (ifc==0) {
        *alt_ifc=0;
        return TRUE;
    }
    else {
        return FALSE;
    }
}

BOOL handle_set_interface(BYTE ifc, BYTE alt_ifc) {
    if (ifc == 0 && alt_ifc == 0) {
        // SEE TRM 2.3.7
        RESETTOGGLE(0x02);
        RESETTOGGLE(0x86);
        RESETFIFO(0x02);
        EP2BCL = 0x80;
        SYNCDELAY();
        EP2BCL = 0x80;
        SYNCDELAY();
        RESETFIFO(0x86);
        return TRUE;
    }

    return FALSE;
}

BYTE handle_get_configuration() {
    return 1;
}

BOOL handle_set_configuration(BYTE cfg) {
    return cfg==1 ? TRUE : FALSE; // we only handle cfg 1
}

void handle_reset_ep(BYTE ep) {
    // silence warning
    ep = ep;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------

// TODO: Add sut_isr

void sudav_isr() __interrupt SUDAV_ISR {
    got_sud=TRUE;
    CLEAR_SUDAV();
}

void usbreset_isr() __interrupt USBRESET_ISR {
    handle_hispeed(FALSE);
    CLEAR_USBRESET();
}

void hispeed_isr() __interrupt HISPEED_ISR {
    handle_hispeed(TRUE);
    CLEAR_HISPEED();
}

void resume_isr() __interrupt RESUME_ISR {
    CLEAR_RESUME();
}

void suspend_isr() __interrupt SUSPEND_ISR {
    dosuspend=TRUE;
    CLEAR_SUSPEND();
}
