#include "usb_setup.h"

#include <cstdio>

#include "esp_log.h"
#include "esp_mac.h"
#include "hidReportDesc.h"
#include "tinyusb.h"
#include "tinyusb_default_config.h"
#include "wheel_registry.h"

static const char *TAG = "usb_setup";
//--------------------------------------------------------------------+
// Device Descriptors
//--------------------------------------------------------------------+
tusb_desc_device_t const desc_device = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = 0x00,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor = wheel_table[WHEEL_LG_G923_XONE].vid,
    .idProduct = wheel_table[WHEEL_LG_G923_XONE].pid,
    .bcdDevice = 0x0100,
    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x03,
    .bNumConfigurations = 0x01,
};
//--------------------------------------------------------------------+
// HID Report Descriptor
//--------------------------------------------------------------------+
// Invoked when received GET HID REPORT DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance) {
    (void)instance;
    return desc_hid_report;
}
//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+
enum { ITF_NUM_HID, ITF_NUM_TOTAL };
#define TUSB_DESC_IN_OUT_TOTAL_LEN (TUD_CONFIG_DESC_LEN + CFG_TUD_HID * TUD_HID_INOUT_DESC_LEN)
#define EPNUM_HID 0x81
uint8_t const desc_configuration[] = {
    // Config number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, TUSB_DESC_IN_OUT_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_SELF_POWERED, 100),
    // Interface number, string index, protocol, report descriptor len, EP In address, size & polling interval
    TUD_HID_INOUT_DESCRIPTOR(ITF_NUM_HID, 0, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report), EPNUM_HID, (EPNUM_HID & 0x0f), CFG_TUD_HID_EP_BUFSIZE, USB_POLLING_INTERVAL),
};
//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+
// String Descriptor Index
enum {
    STRID_LANGID = 0,
    STRID_MANUFACTURER,
    STRID_PRODUCT,
    STRID_SERIAL,
};
// array of pointer to string descriptors
char const *string_desc_arr[] = {
    (const char[]){0x09, 0x04},  // 0: is supported language is English (0x0409)
    "TinyUSB",                   // 1: Manufacturer
    "TinyUSB Device",            // 2: Product
    NULL,                        // 3: Serials will use unique ID if possible
};
static char usb_serial[13];
static void generate_usb_serial(void) {
    uint8_t mac[6];
    esp_efuse_mac_get_default(mac);
    snprintf(usb_serial, sizeof(usb_serial), "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    ESP_LOGI(TAG, "USB Serial: %s", usb_serial);
}
void usb_setup(void) {
    ESP_LOGI(TAG, "USB initialization");
    generate_usb_serial();
    string_desc_arr[3] = usb_serial;
    tinyusb_config_t tusb_cfg = TINYUSB_DEFAULT_CONFIG();
    // tusb_cfg.descriptor.device = NULL;
    tusb_cfg.descriptor.device = &desc_device;
    tusb_cfg.descriptor.full_speed_config = desc_configuration;
    tusb_cfg.descriptor.string = string_desc_arr;
    tusb_cfg.descriptor.string_count = sizeof(string_desc_arr) / sizeof(string_desc_arr[0]);
#if (TUD_OPT_HIGH_SPEED)
    tusb_cfg.descriptor.high_speed_config = desc_configuration;
#endif  // TUD_OPT_HIGH_SPEED
    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));
    ESP_LOGI(TAG, "USB initialization DONE");
}
