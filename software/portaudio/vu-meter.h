#ifndef VU_METER_H
#define VU_METER_H

void vu_meter_init();
void vu_meter_on_sample(int channels, int count, float *buffer);

// vim: set ts=8 sw=4 sts=4 expandtab:
#endif
