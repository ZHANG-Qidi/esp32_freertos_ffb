#ifndef _FFB_LOOP_H_
#define _FFB_LOOP_H_
#include "FFB_config.h"
#ifdef __cplusplus
extern "C" {
#endif
#define MOTOR_DAMPING_MIN (CONFIG_FFB_MIN_DAMPING)
extern void ffb_output(float *constant_force, float *damper);
extern void ffb_loop(void);
#ifdef __cplusplus
}
#endif
#endif
