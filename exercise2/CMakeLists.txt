add_executable(exercise2
    main.c
)

target_link_libraries(exercise2 pico_stdlib)
pico_enable_stdio_usb(exercise2 1)
pico_enable_stdio_uart(exercise2 0)
pico_add_extra_outputs(exercise2)
