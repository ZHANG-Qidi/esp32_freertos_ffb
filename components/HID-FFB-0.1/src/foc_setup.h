#ifndef _FOC_SETUP_H_
#define _FOC_SETUP_H_
#include "Arduino_config.h"
#include "FFB_config.h"
#include "SimpleFOC.h"
#ifdef __cplusplus
extern "C" {
#endif
#define BLDC_MOTOR_PP (CONFIG_FFB_MOTOR_POLE_PAIRS)
#define VOLTAGE_POWER (CONFIG_FFB_POWER_SUPPLY_VOLTAGE)
#define VOLTAGE_LIMIT (CONFIG_FFB_MOTOR_VOLTAGE_LIMIT)
#define VOLTAGE_SENSOR_ALIGN (CONFIG_FFB_SENSOR_ALIGN_VOLTAGE)
#define COMMANDER_BAUD_RATE (CONFIG_FFB_COMMANDER_BAUD_RATE)
#define MOTOR_U (CONFIG_FOC_MOTOR_U)
#define MOTOR_V (CONFIG_FOC_MOTOR_V)
#define MOTOR_W (CONFIG_FOC_MOTOR_W)
#define MOTOR_EN (CONFIG_FOC_MOTOR_EN)
extern BLDCMotor motor;
#if defined(CONFIG_FFB_MagneticSensorSPI)
extern MagneticSensorSPI sensor;
#define CONFIG_FFB_SENSOR_DIRECTION (-1.0f)
#endif
#if defined(CONFIG_FFB_MagneticSensorIIC)
extern MagneticSensorI2C sensor;
#define CONFIG_FFB_SENSOR_DIRECTION (1.0f)
#endif
extern void foc_setup(void);
#ifdef __cplusplus
}
#endif
#endif
