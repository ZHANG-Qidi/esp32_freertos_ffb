#include "ffb_loop.h"

#include "esp_log.h"
#include "ffb_setup.h"
static const char *TAG = "ffb_loop";
static float g_constant_force;
static float g_damper = MOTOR_DAMPING_MIN;
void ffb_output(float *constant_force, float *damper) {
    *constant_force = g_constant_force;
    *damper = g_damper;
}
void ffb_loop(void) {
    for (int i = 0; i < FFB_EFFECT_COUNT; i++) {
        if (g_effect_pool[i].allocated == BLOCK_FREE) {
            continue;
        }
        if (g_effect_pool[i].operation_report.operation == EFFECT_STOP) {
            continue;
        }
        // float duration_ms = g_effect_pool[i].effect_report.duration * 0.1f;
        // float trigger_repeat_interval_ms = g_effect_pool[i].effect_report.trigger_repeat_interval * 0.1f;
        // float sample_period_ms = g_effect_pool[i].effect_report.sample_period * 0.1f;
        // float direction_deg = g_effect_pool[i].effect_report.direction[0] / 255.0f * 360.0f;
        float gain_effect_scale = g_effect_pool[i].effect_report.gain / GAIN_EFFECT_LOGICAL_MAX;
        float gain_device_scale = g_gain_device / GAIN_DEVICE_LOGICAL_MAX;
        switch (g_effect_pool[i].effect_report.type) {
            case ET_CONSTANT: {
                int16_t magnitude = g_effect_pool[i].constant_force_report.magnitude;
                float constant_force = magnitude * gain_effect_scale * gain_device_scale / CONSTANT_MANITUDE_MAX * MOTOR_GAIN_CONSTANT;
                g_constant_force = constant_force;
                // ESP_LOGI(TAG, "ET_CONSTANT: %f", g_constant_force);
                break;
            }
            case ET_DAMPER: {
                float damper = (float)g_effect_pool[i].condition_report.positive_coefficient / (float)g_effect_pool[i].condition_report.positive_saturation * MOTOR_GAIN_DAMPING;
                g_damper = damper < MOTOR_DAMPING_MIN ? MOTOR_DAMPING_MIN : damper;
                // ESP_LOGI(TAG, "ET_DAMPER: %f", g_damper);
                break;
            }
            case ET_DAMPER_DR2: {
                float damper = (float)g_effect_pool[i].condition_report.positive_coefficient / (float)g_effect_pool[i].condition_report.positive_saturation * MOTOR_GAIN_DAMPING_DR2;
                g_damper = damper < MOTOR_DAMPING_MIN ? MOTOR_DAMPING_MIN : damper;
                // ESP_LOGI(TAG, "ET_DAMPER: %f", g_damper);
                break;
            }
            default: {
                ESP_LOGI(TAG, "Unimplemented Effect: %d", g_effect_pool[i].effect_report.type);
                break;
            }
        }
    }
}
