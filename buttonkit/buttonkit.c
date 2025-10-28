#include "buttonkit.h"

#define DEBOUNCE_MS    30
#define LONG_PRESS_MS 3000

static uint g_button_pin;
static bool stable_level = true;
static bool last_sample  = true;
static absolute_time_t last_change_time;
static bool pressed = false;
static absolute_time_t press_start_time;
static bool long_fired = false;
static bool short_trigger = false;
static bool long_trigger = false;

void buttonkit_init(uint pin) {
    g_button_pin = pin;
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_pull_up(pin);
    bool lvl = gpio_get(pin);
    stable_level = last_sample = lvl;
    pressed = (lvl == 0);
    last_change_time = get_absolute_time();
    press_start_time = get_absolute_time();
}

bool buttonkit_poll(void) {
    bool sample = gpio_get(g_button_pin);
    absolute_time_t now = get_absolute_time();
    short_trigger = false;
    long_trigger = false;

    // debounce
    if (sample != last_sample) {
        last_sample = sample;
        last_change_time = now;
        return pressed;
    }

    if (absolute_time_diff_us(last_change_time, now) >= DEBOUNCE_MS * 1000) {
        if (stable_level != sample) {
            stable_level = sample;
            if (sample == 0) {
                pressed = true;
                press_start_time = now;
                long_fired = false;
            } else {
                if (pressed) {
                    pressed = false;
                    if (!long_fired) {
                        int64_t press_ms = absolute_time_diff_us(press_start_time, now) / 1000;
                        if (press_ms >= LONG_PRESS_MS) {
                            long_trigger = true;
                        } else {
                            short_trigger = true;
                        }
                    }
                }
            }
        }
    }

    if (pressed && !long_fired) {
        int64_t held_ms = absolute_time_diff_us(press_start_time, now) / 1000;
        if (held_ms >= LONG_PRESS_MS) {
            long_trigger = true;
            long_fired = true;
        }
    }

    return pressed;
}

bool buttonkit_was_short_pressed(void) { return short_trigger; }
bool buttonkit_was_long_pressed(void)  { return long_trigger;  }
