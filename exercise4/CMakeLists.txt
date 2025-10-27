add_executable(exercise4
    main.c
)

target_link_libraries(exercise4 pico_stdlib)
pico_enable_stdio_usb(exercise4 1)
pico_enable_stdio_uart(exercise4 0)
pico_add_extra_outputs(exercise4)
