file(GLOB_RECURSE HID_FFB_SRC
    ${CMAKE_CURRENT_LIST_DIR}/*.c
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
)

idf_component_register(
    SRCS
        ${HID_FFB_SRC}
    INCLUDE_DIRS
        ${CMAKE_CURRENT_LIST_DIR}/src
    PRIV_REQUIRES
        esp_tinyusb
    REQUIRES
        Arduino-FOC-2.2
        ArduinoCore-avr-1.8.7
)

target_compile_options(${COMPONENT_LIB} PRIVATE
    -fpermissive
    -Wno-missing-field-initializers
)
