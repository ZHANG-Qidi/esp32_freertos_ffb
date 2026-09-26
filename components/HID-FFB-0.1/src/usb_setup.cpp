#include "usb_setup.h"

#include "bsp/board_api.h"
#include "hidReportDesc.h"
#include "tusb.h"
#include "wheel_registry.h"
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
// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const *tud_descriptor_device_cb(void) { return (uint8_t const *)&desc_device; }
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
// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const *tud_descriptor_configuration_cb(uint8_t index) {
    (void)index;  // for multiple configurations
    // This example use the same configuration for both high and full speed mode
    return desc_configuration;
}
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
//--------------------------------------------------------------------+
// Weak board API (to be optionally implemented by board)
//--------------------------------------------------------------------+
TU_ATTR_WEAK size_t board_get_unique_id(uint8_t id[], size_t max_len) {
    (void)max_len;
    // fixed serial string is 01234567889ABCDEF
    uint32_t *uid32 = (uint32_t *)(uintptr_t)id;
    uid32[0] = 0x67452301u;
    uid32[1] = 0xEFCDAB89u;
    return 8;
}
static uint16_t _desc_str[32 + 1];
// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    (void)langid;
    size_t chr_count;
    switch (index) {
        case STRID_LANGID:
            memcpy(&_desc_str[1], string_desc_arr[0], 2);
            chr_count = 1;
            break;
        case STRID_SERIAL:
            chr_count = board_usb_get_serial(_desc_str + 1, 32);
            break;
        default:
            // Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
            // https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors
            if (!(index < sizeof(string_desc_arr) / sizeof(string_desc_arr[0]))) return NULL;
            const char *str = string_desc_arr[index];
            // Cap at max char
            chr_count = strlen(str);
            size_t const max_count = sizeof(_desc_str) / sizeof(_desc_str[0]) - 1;  // -1 for string type
            if (chr_count > max_count) chr_count = max_count;
            // Convert ASCII string into UTF-16
            for (size_t i = 0; i < chr_count; i++) {
                _desc_str[1 + i] = str[i];
            }
            break;
    }
    // first byte is length (including header), second byte is string type
    _desc_str[0] = (uint16_t)((TUSB_DESC_STRING << 8) | (2 * chr_count + 2));
    return _desc_str;
}
void usb_setup(void) {
    // init device stack on configured roothub port
    // This should be called after scheduler/kernel is started.
    // Otherwise it could cause kernel issue since USB IRQ handler does use RTOS queue API.
    tusb_rhport_init_t dev_init = {.role = TUSB_ROLE_DEVICE, .speed = TUSB_SPEED_AUTO};
    tusb_init(BOARD_TUD_RHPORT, &dev_init);
}
