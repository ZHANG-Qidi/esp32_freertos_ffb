file(GLOB_RECURSE ARDUINO_FOC_SRC
    ${CMAKE_CURRENT_LIST_DIR}/*.c
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
)

idf_component_register(
    SRCS
        ${ARDUINO_FOC_SRC}
    INCLUDE_DIRS
        ${CMAKE_CURRENT_LIST_DIR}/src
        ${CMAKE_SOURCE_DIR}/main
    PRIV_REQUIRES
        esp_driver_gpio
        esp_driver_uart
        esp_driver_i2c
        esp_driver_spi
        esp_driver_ledc
        esp_driver_mcpwm
        esp_timer
        ArduinoCore-avr-1.8.7
)
