add_executable(pwm)

target_sources(pwm PRIVATE pwm.c picoUtils.c)

target_link_libraries(pwm PRIVATE pico_stdlib hardware_pwm)

pico_enable_stdio_usb(pwm 1)
pico_enable_stdio_uart(pwm 0)

pico_add_extra_outputs(pwm)

