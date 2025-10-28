#ifndef BUTTONKIT_H
#define BUTTONKIT_H

#include "pico/stdlib.h"
#include <stdbool.h>

void buttonkit_init(uint pin);
bool buttonkit_poll(void);
bool buttonkit_was_short_pressed(void);
bool buttonkit_was_long_pressed(void);

#endif
