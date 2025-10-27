add_executable(exercise1
    main.c
)

target_link_libraries(exercise1 pico_stdlib)
pico_enable_stdio_usb(exercise1 1)
pico_enable_stdio_uart(exercise1 0)
pico_add_extra_outputs(exercise1)
