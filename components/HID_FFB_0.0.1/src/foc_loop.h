#ifndef _FOC_LOOP_H_
#define _FOC_LOOP_H_
#ifdef __cplusplus
extern "C" {
#endif
#define DAMPING_MAX_VELOCITY (6.0f * PI)
#define FOC_LOOP_PERIOD (200.f)
#define SENSOR_DIRECTION (-1.0f)
#define SENSOR_STEP_MIN (0.0003835f)
#define SENSOR_STEP_NUM (2.0f)
extern void foc_output(float *wheel_rad);
extern void foc_loop(void);
#ifdef __cplusplus
}
#endif
#endif
