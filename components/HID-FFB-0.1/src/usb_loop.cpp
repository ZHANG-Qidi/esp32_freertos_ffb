#include "usb_loop.h"

#include "adc_loop.h"
#include "esp_log.h"
#include "ffb_setup.h"
#include "foc_loop.h"
#include "hidReportDesc.h"
#include "tusb.h"

static const char *TAG = "usb_loop";
static void dump_hex(const uint8_t *buf, int len) {
    char line[256];
    int pos = 0;
    for (int i = 0; i < len; i++) {
        pos += snprintf(line + pos, sizeof(line) - pos, "%02X ", buf[i]);
    }
    ESP_LOGI(TAG, "%s", line);
}
/* Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum {
    BLINK_NOT_MOUNTED = 250,
    BLINK_MOUNTED = 1000,
    BLINK_SUSPENDED = 2500,
};
uint32_t blink_interval_ms_usb_status = BLINK_NOT_MOUNTED;
//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+
// Invoked when device is mounted
void tud_mount_cb(void) { blink_interval_ms_usb_status = BLINK_MOUNTED; }
// Invoked when device is unmounted
void tud_umount_cb(void) { blink_interval_ms_usb_status = BLINK_NOT_MOUNTED; }
// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en) {
    (void)remote_wakeup_en;
    blink_interval_ms_usb_status = BLINK_SUSPENDED;
}
// Invoked when usb bus is resumed
void tud_resume_cb(void) { blink_interval_ms_usb_status = tud_mounted() ? BLINK_MOUNTED : BLINK_NOT_MOUNTED; }
// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report, uint16_t len) {
    (void)instance;
    (void)report;
    (void)len;
}
// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen) {
    ESP_LOGI(TAG, "GET_REPORT: inst=%u id=%u type=%u len=%u", instance, report_id, report_type, reqlen);
    dump_hex(buffer, reqlen);
    if (report_type == HID_REPORT_TYPE_INPUT) {
    }
    if (report_type == HID_REPORT_TYPE_OUTPUT) {
    }
    if (report_type == HID_REPORT_TYPE_FEATURE) {
        return ffb_hid_get_feature(report_id, buffer);
    }
    return 0;
}
// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize) {
    // ESP_LOGI(TAG, "SET_REPORT: inst=%u id=%u type=%u size=%u", instance, report_id, report_type, bufsize);
    // dump_hex(buffer, bufsize);
    if (report_type == HID_REPORT_TYPE_INPUT) {
        // ESP_LOGI(TAG, "SET_REPORT: inst=%u id=%u type=%u size=%u", instance, report_id, report_type, bufsize);
        // dump_hex(buffer, bufsize);
    }
    if (report_type == HID_REPORT_TYPE_OUTPUT) {
        // ESP_LOGI(TAG, "SET_REPORT: inst=%u id=%u type=%u size=%u", instance, report_id, report_type, bufsize);
        // dump_hex(buffer, bufsize);
        ffb_hid_set_output(buffer);
    }
    if (report_type == HID_REPORT_TYPE_FEATURE) {
        ESP_LOGI(TAG, "SET_REPORT: inst=%u id=%u type=%u size=%u", instance, report_id, report_type, bufsize);
        dump_hex(buffer, bufsize);
        ffb_hid_set_feature(report_id, buffer);
    }
}
void usb_loop(void) {
    if (!(tud_mounted() && tud_hid_ready())) {
        return;
    }
    hid_joystick_input_t joy = {
        .axis_x = (uint32_t)JOYSTIC_AXIS_LOGICAL_MID,
        .axis_y = (uint32_t)JOYSTIC_AXIS_LOGICAL_MID,
        .axis_z = (uint32_t)JOYSTIC_AXIS_LOGICAL_MID,
        .axis_rx = (uint32_t)JOYSTIC_AXIS_LOGICAL_MID,
        .axis_ry = (uint32_t)JOYSTIC_AXIS_LOGICAL_MID,
        .axis_rz = (uint32_t)JOYSTIC_AXIS_LOGICAL_MID,
        .slider = (uint32_t)JOYSTIC_AXIS_LOGICAL_MID,
        .dial = (uint32_t)JOYSTIC_AXIS_LOGICAL_MID,
        .pov = 8,
    };
    float wheel_rad;
    foc_output(&wheel_rad);
    float wheel_rad_clamped = wheel_rad > WHEEL_HALF ? WHEEL_HALF : (wheel_rad < -WHEEL_HALF ? -WHEEL_HALF : wheel_rad);
    joy.axis_x = JOYSTIC_AXIS_LOGICAL_MID + wheel_rad_clamped / WHEEL_HALF * JOYSTIC_AXIS_LOGICAL_MID;
    joy.axis_rx = JOYSTIC_AXIS_LOGICAL_MAX * (1.0f - adc_output(0));
    joy.axis_ry = JOYSTIC_AXIS_LOGICAL_MAX * adc_output(1);
    joy.axis_rz = JOYSTIC_AXIS_LOGICAL_MAX * adc_output(2);
    tud_hid_report(TLID, &joy, sizeof(hid_joystick_input_t));
}
