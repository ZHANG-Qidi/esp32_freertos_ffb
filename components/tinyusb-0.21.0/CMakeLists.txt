file(GLOB_RECURSE TINYUSB_SRC
    ${CMAKE_CURRENT_LIST_DIR}/src/*.c
)

idf_component_register(
    SRCS
        ${TINYUSB_SRC}
    INCLUDE_DIRS
        ${CMAKE_CURRENT_LIST_DIR}/src
        ${CMAKE_CURRENT_LIST_DIR}/hw
        ${CMAKE_SOURCE_DIR}/main
    PRIV_REQUIRES
        esp_driver_gpio
)
