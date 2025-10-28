
add_library(buttonkit STATIC
    buttonkit.c
)

target_include_directories(buttonkit
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(buttonkit
    PUBLIC
        pico_stdlib
        hardware_gpio
        hardware_pwm
)
