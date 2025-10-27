add_executable(exercise3
    main.c
)

target_link_libraries(exercise3 pico_stdlib)
pico_enable_stdio_usb(exercise3 1)
pico_enable_stdio_uart(exercise3 0)
pico_add_extra_outputs(exercise3)
