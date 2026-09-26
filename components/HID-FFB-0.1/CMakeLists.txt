file(GLOB_RECURSE HID_FFB_SRC
    ${CMAKE_CURRENT_LIST_DIR}/*.c
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
)

idf_component_register(
    SRCS
        ${HID_FFB_SRC}
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
        tinyusb-0.21.0
        Arduino-FOC-2.2
        ArduinoCore-avr-1.8.7
)

target_compile_options(${COMPONENT_LIB} PRIVATE
    -fpermissive
    -Wno-missing-field-initializers
)
