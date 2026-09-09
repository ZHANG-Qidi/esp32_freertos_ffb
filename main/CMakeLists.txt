idf_component_register(
    SRCS
    "main.cpp"
    INCLUDE_DIRS
    "."
    PRIV_REQUIRES
    nvs_flash
    esp_event
    esp_netif
    esp_wifi
    esp_driver_gpio
    esp_driver_uart
    esp_driver_gptimer
    REQUIRES
    ArduinoCore-avr-1.8.7
    Arduino-FOC-2.2
    HID_FFB_0.0.1
)

target_compile_options(${COMPONENT_LIB} PRIVATE
    -fpermissive
    -Wno-missing-field-initializers
)
