#ifndef _FFB_LOOP_H_
#define _FFB_LOOP_H_
#include "freertos_port.h"
#ifdef __cplusplus
extern "C" {
#endif
#define MOTOR_DAMPING_MIN (0.1f)
extern void ffb_output(float *constant_force, float *damper);
extern void ffb_loop(void);
#ifdef __cplusplus
}
#endif
#endif
