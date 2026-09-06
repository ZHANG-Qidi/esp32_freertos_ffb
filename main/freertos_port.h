#ifndef _FREERTOS_PORT_H_
#define _FREERTOS_PORT_H_

#if defined(ESP_PLATFORM) || defined(ESP_IDF)
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#else
#include "FreeRTOS.h"
#include "task.h"
#endif

#endif