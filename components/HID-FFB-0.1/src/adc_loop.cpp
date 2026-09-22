#include "adc_loop.h"

#include <cstdint>

#include "Arduino.h"

// static const char *TAG = "adc_loop";

typedef struct {
    uint16_t data;
    uint16_t min;
    uint16_t max;
} adc_data_type;

static adc_data_type adc_data[3] = {
    {.min = 65535},
    {.min = 65535},
    {.min = 65535},
};

void adc_loop(void) {
    for (int i = 0; i < 3; i++) {
        adc_data[i].data = analogRead(i);
        adc_data[i].min = adc_data[i].data < adc_data[i].min ? adc_data[i].data : adc_data[i].min;
        adc_data[i].max = adc_data[i].data > adc_data[i].max ? adc_data[i].data : adc_data[i].max;
    }
}

#define DEAD_ZONE_LOW (0.10)
#define DEAD_ZONE_HIGH (0.90)

float adc_output(int index) {
    float adc_value = (adc_data[index].data - adc_data[index].min) / (float)(adc_data[index].max - adc_data[index].min);
    adc_value = (adc_value - DEAD_ZONE_LOW) / (DEAD_ZONE_HIGH - DEAD_ZONE_LOW);
    adc_value = adc_value < 0 ? 0 : adc_value;
    adc_value = adc_value > 1 ? 1 : adc_value;
    return adc_value;
}
