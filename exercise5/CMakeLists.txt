add_executable(exercise5
    main.c
)

target_link_libraries(exercise5
    PRIVATE
        buttonkit
)

pico_enable_stdio_usb(exercise5 1)
pico_enable_stdio_uart(exercise5 0)
pico_add_extra_outputs(exercise5)
