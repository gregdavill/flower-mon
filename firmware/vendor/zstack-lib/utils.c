#include "utils.h"
#include "hal_adc.h"

// #define MAX(x, y) (((x) > (y)) ? (x) : (y))
// #define MIN(x, y) (((x) < (y)) ? (x) : (y))

float mapRange(float a1, float a2, float b1, float b2, float s) {
    float result = b1 + (s - a1) * (b2 - b1) / (a2 - a1);
    return MIN(b2, MAX(result, b1));
}

uint16 adcReadSampled(uint8 channel, uint8 resolution, uint8 reference, uint8 samplesCount) {
    HalAdcSetReference(reference);
    uint32 samplesSum = 0;
    for (uint8 i = 0; i < samplesCount; i++) {
        samplesSum += HalAdcRead(channel, resolution);
    }
    return samplesSum /samplesCount;
}
