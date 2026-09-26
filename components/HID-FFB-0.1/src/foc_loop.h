#ifndef _FOC_LOOP_H_
#define _FOC_LOOP_H_
#include "FFB_config.h"
#ifdef __cplusplus
extern "C" {
#endif
#define DAMPING_MAX_VELOCITY (CONFIG_FFB_DAMPING_MAX_VELOCITY_RAD_S)
#define FOC_LOOP_PERIOD (CONFIG_FFB_FOC_LOOP_PERIOD_US)
#define SENSOR_DIRECTION (CONFIG_FFB_SENSOR_DIRECTION)
#define SENSOR_STEP_MIN (CONFIG_FFB_SENSOR_STEP_MIN_RAD)
#define SENSOR_STEP_NUM (CONFIG_FFB_SENSOR_STEP_COUNT)
extern void foc_output(float *wheel_rad);
extern void foc_loop(void);
#ifdef __cplusplus
}
#endif
#endif
