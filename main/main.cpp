#include "main.h"

#include <stdio.h>

#include "driver/gpio.h"
#include "driver/gptimer.h"
#include "ffb_loop.h"
#include "ffb_setup.h"
#include "foc_loop.h"
#include "foc_setup.h"
#include "led_strip.h"
#include "tusb.h"
#include "usb_loop.h"
#include "usb_setup.h"
static const char *TAG = "main";
/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define BLINK_GPIO CONFIG_BLINK_GPIO
static uint8_t s_led_state = 0;
#ifdef CONFIG_BLINK_LED_STRIP
static led_strip_handle_t led_strip;
static void blink_loop(void) {
    /* If the addressable LED is enabled */
    if (s_led_state) {
        /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
        led_strip_set_pixel(led_strip, 0, 0, 2, 0);
        /* Refresh the strip to send data */
        led_strip_refresh(led_strip);
    } else {
        /* Set all LED off to clear all pixels */
        led_strip_clear(led_strip);
    }
}
static void blink_setup(void) {
    ESP_LOGI(TAG, "Example configured to blink addressable LED!");
    /* LED strip initialization with the GPIO and pixels number*/
    led_strip_config_t strip_config = {
        .strip_gpio_num = BLINK_GPIO,
        .max_leds = 1,  // at least one LED on board
    };
#if CONFIG_BLINK_LED_STRIP_BACKEND_RMT
    led_strip_rmt_config_t rmt_config = {};
    rmt_config.resolution_hz = 10 * 1000 * 1000;  // 10MHz
    rmt_config.flags.with_dma = false;
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
#elif CONFIG_BLINK_LED_STRIP_BACKEND_SPI
    led_strip_spi_config_t spi_config = {
        .spi_bus = SPI2_HOST,
        .flags.with_dma = true,
    };
    ESP_ERROR_CHECK(led_strip_new_spi_device(&strip_config, &spi_config, &led_strip));
#else
#error "unsupported LED strip backend"
#endif
    /* Set all LED off to clear all pixels */
    led_strip_clear(led_strip);
}
#elif CONFIG_BLINK_LED_GPIO
static void blink_loop(void) {
    /* Set the GPIO level according to the state (LOW or HIGH)*/
    gpio_set_level(BLINK_GPIO, s_led_state);
}
static void blink_setup(void) {
    ESP_LOGI(TAG, "Example configured to blink GPIO LED!");
    gpio_reset_pin(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
}
#else
#error "unsupported LED type"
#endif
void blink_task(__unused void *params) {
    TickType_t last = xTaskGetTickCount();
    for (;;) {
        vTaskDelayUntil(&last, pdMS_TO_TICKS(blink_interval_ms_usb_status));
        s_led_state = !s_led_state;
        blink_loop();
    }
}
TaskHandle_t foc_task_handle = NULL;
static bool example_timer_on_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(foc_task_handle, &xHigherPriorityTaskWoken);
    return (xHigherPriorityTaskWoken == pdTRUE);
}
void foc_task(__unused void *params) {
    foc_setup();
    for (;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        foc_loop();
    }
}
void usb_task(__unused void *params) {
    usb_setup();
    TickType_t last = xTaskGetTickCount();
    for (;;) {
        vTaskDelayUntil(&last, pdMS_TO_TICKS(USB_POLLING_INTERVAL));
        usb_loop();
    }
}
void ffb_task(__unused void *params) {
    TickType_t last = xTaskGetTickCount();
    for (;;) {
        vTaskDelayUntil(&last, pdMS_TO_TICKS(USB_POLLING_INTERVAL));
    }
}
// Priorities of our threads - higher numbers are higher priority
#define MAIN_TASK_PRIORITY (tskIDLE_PRIORITY + osPriorityBelowNormal)
#define BLINK_TASK_PRIORITY (tskIDLE_PRIORITY + osPriorityLow)
#define WORKER_TASK_PRIORITY (tskIDLE_PRIORITY + osPriorityNormal)
// Stack sizes of our threads in words (4 bytes)
#define MAIN_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)
#define BLINK_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)
#define WORKER_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)
void gptimer_init(void) {
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1 * 1000 * 1000,
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));
    gptimer_alarm_config_t alarm_config = {};
    alarm_config.reload_count = 0;
    alarm_config.alarm_count = FOC_LOOP_PERIOD;
    alarm_config.flags.auto_reload_on_alarm = true;
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
    gptimer_event_callbacks_t cbs = {
        .on_alarm = example_timer_on_alarm_cb,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, NULL));
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    ESP_ERROR_CHECK(gptimer_start(gptimer));
}
extern "C" void app_main(void) {
    blink_setup();
    xTaskCreate(blink_task, "blink_task", BLINK_TASK_STACK_SIZE, NULL, BLINK_TASK_PRIORITY, NULL);
    xTaskCreate(foc_task, "foc_task", WORKER_TASK_STACK_SIZE, NULL, WORKER_TASK_PRIORITY, &foc_task_handle);
    xTaskCreate(usb_task, "usb_task", WORKER_TASK_STACK_SIZE, NULL, WORKER_TASK_PRIORITY, NULL);
    xTaskCreate(ffb_task, "ffb_task", WORKER_TASK_STACK_SIZE, NULL, WORKER_TASK_PRIORITY, NULL);
    gptimer_init();
    TickType_t last = xTaskGetTickCount();
    while (true) {
        vTaskDelayUntil(&last, pdMS_TO_TICKS(10));
    }
}
