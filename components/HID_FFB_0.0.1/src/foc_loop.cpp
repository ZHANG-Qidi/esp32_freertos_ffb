#include "foc_loop.h"

#include "SimpleFOC.h"
#include "esp_log.h"
#include "ffb_loop.h"
#include "foc_setup.h"
#include "freertos_port.h"
// static const char *TAG = "foc_loop";
static float wheel_rad_g;
void foc_output(float *wheel_rad) { *wheel_rad = wheel_rad_g; }
void foc_loop(void) {
    // main FOC algorithm function
    // the faster you run this function the better
    // Arduino UNO loop  ~1kHz
    // Bluepill loop ~10kHz
    motor.loopFOC();
    float damper;
    float constant_force;
    ffb_output(&constant_force, &damper);
    float damping = damper * motor.shaft_velocity / DAMPING_MAX_VELOCITY;
    float torque_ratio = constant_force - damping;
    torque_ratio = torque_ratio > 1.0f ? 1.0f : (torque_ratio < -1.0f ? -1.0f : torque_ratio);
    // voltage set point variable
    float target_voltage = VOLTAGE_LIMIT * torque_ratio;
    // Motion control function
    // current_velocity, position or voltage (defined in motor.controller)
    // this function can be run at much lower frequency than loopFOC() function
    // You can also use motor.move() and set the motor.target in the code
    motor.move(target_voltage);
    wheel_rad_g = sensor.getAngle() * SENSOR_DIRECTION;
    static int dividerCounter;
    const int dividerFactor = 10.f * 1000.f / FOC_LOOP_PERIOD;
    if (++dividerCounter < dividerFactor) {
        return;
    }
    dividerCounter = 0;
    // ESP_LOGI(TAG, "damper = %f, constant_force = %f", damper, constant_force);
}
