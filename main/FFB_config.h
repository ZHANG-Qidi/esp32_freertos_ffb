#ifndef _FFB_CONFIG_H_
#define _FFB_CONFIG_H_

// Force-feedback device and motor-control tuning. These values are independent
// of the MCU peripheral and GPIO assignments in Arduino_config.h.

// Motor and sensor setup.
#define CONFIG_FFB_MOTOR_POLE_PAIRS 7
#define CONFIG_FFB_POWER_SUPPLY_VOLTAGE 9.0f
#define CONFIG_FFB_MOTOR_VOLTAGE_LIMIT 6.0f
#define CONFIG_FFB_SENSOR_ALIGN_VOLTAGE 1.0f
#define CONFIG_FFB_COMMANDER_BAUD_RATE 115200
#define CONFIG_FFB_MagneticSensorSPI
// #define CONFIG_FFB_MagneticSensorIIC

// Control-loop timing and steering travel.
#define CONFIG_FFB_FOC_LOOP_PERIOD_US 200.0f
#define CONFIG_FFB_DAMPING_MAX_VELOCITY_RAD_S (6.0f * 3.14159265358979323846f)
#define CONFIG_FFB_WHEEL_HALF_RANGE_RAD (3.14159265358979323846f * 1.5f)
#define CONFIG_FFB_SENSOR_STEP_MIN_RAD 0.0003835f
#define CONFIG_FFB_SENSOR_STEP_COUNT 2.0f
#define CONFIG_FFB_STOP_ZONE_RAD 0.1f
#define CONFIG_FFB_STOP_FORCE 0.3f

// Force-feedback output tuning.
#define CONFIG_FFB_DAMPING_GAIN 1.0f
#define CONFIG_FFB_DAMPING_DR2_GAIN 0.5f
#define CONFIG_FFB_CONSTANT_FORCE_GAIN 0.2f
#define CONFIG_FFB_MIN_DAMPING 0.1f

#endif
