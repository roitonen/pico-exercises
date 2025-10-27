// File: main.c
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

// ===== Pins (match your Wokwi JSON) =====
#define LED_PIN        15   // LED via resistor to GND, PWM capable
#define BUTTON_PIN     14   // Button to GND, internal pull-up (active LOW)

// ===== Debounce & timings =====
#define DEBOUNCE_MS       30
#define LONG_PRESS_MS   3000
#define BLINK_HZ           2   // arbitrary blink frequency

// ===== PWM config (~1 kHz) =====
#define PWM_WRAP       1000     // duty in promille (0..1000)
#define PWM_CLKDIV      125     // 125 MHz / 125 / (1000+1) ≈ 1 kHz

// ===== LED intensities =====
#define DUTY_OFF           0
#define DUTY_ON        PWM_WRAP
#define DUTY_DIM       (PWM_WRAP / 4) // 25%

// ===== State machine =====
typedef enum {
    STATE_OFF = 0,
    STATE_ON,
    STATE_DIM_25,
    STATE_BLINK
} led_state_t;

static led_state_t current_state = STATE_OFF;
static led_state_t last_non_off_state = STATE_ON; // remembered mode
static bool off_memory_mode = false;              // true only after long-press OFF

// Button debounce / press bookkeeping
static bool btn_stable_level = true; // with pull-up: released=HIGH(true)
static bool btn_last_sample  = true;
static absolute_time_t btn_last_change_time;
static bool btn_pressed = false;     // current stable pressed state
static absolute_time_t press_start_time;
static bool long_fired = false;      // long-press already handled for this hold

// Blink handled in main loop (robust in Wokwi)
static absolute_time_t next_blink_toggle;
static bool blink_is_on = false;

// ===== Helpers =====
static inline void led_set_duty(uint16_t duty) {
    if (duty > PWM_WRAP) duty = PWM_WRAP;
    pwm_set_gpio_level(LED_PIN, duty);
}

static void apply_state(led_state_t s) {
    current_state = s;
    switch (s) {
        case STATE_OFF:
            led_set_duty(DUTY_OFF);
            break;
        case STATE_ON:
            led_set_duty(DUTY_ON);
            last_non_off_state = STATE_ON;
            break;
        case STATE_DIM_25:
            led_set_duty(DUTY_DIM);
            last_non_off_state = STATE_DIM_25;
            break;
        case STATE_BLINK: {
            blink_is_on = false;
            int64_t half_period_ms = 1000 / (BLINK_HZ * 2); // toggle each half period
            next_blink_toggle = make_timeout_time_ms(half_period_ms);
            led_set_duty(DUTY_OFF); // start from OFF, will toggle in loop
            last_non_off_state = STATE_BLINK;
            break;
        }
    }
}

static void next_state_short_press(void) {
    switch (current_state) {
        case STATE_OFF:
            if (off_memory_mode) {
                // Resume last non-off state exactly once after a long-press OFF
                apply_state(last_non_off_state);
                off_memory_mode = false;
            } else {
                // Normal 4-step cycle restarts at ON
                apply_state(STATE_ON);
            }
            break;
        case STATE_ON:
            apply_state(STATE_DIM_25);
            break;
        case STATE_DIM_25:
            apply_state(STATE_BLINK);
            break;
        case STATE_BLINK:
            apply_state(STATE_OFF);
            // OFF reached by normal cycle -> next press must go to ON
            off_memory_mode = false;
            break;
    }
}

static void long_press_action(void) {
    // From any state: turn OFF and enable resume-once behavior
    off_memory_mode = true;
    apply_state(STATE_OFF);
}

// ----- Button init & polling -----
static void button_init(void) {
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN); // button to GND => released=HIGH, pressed=LOW

    bool lvl = gpio_get(BUTTON_PIN);
    btn_stable_level = btn_last_sample = lvl;
    btn_pressed = (lvl == 0);
    btn_last_change_time = get_absolute_time();
    press_start_time = get_absolute_time();
    long_fired = false;
}

static void button_poll(void) {
    bool sample = gpio_get(BUTTON_PIN); // HIGH=released, LOW=pressed
    absolute_time_t now = get_absolute_time();

    // Raw edge: restart debounce timing
    if (sample != btn_last_sample) {
        btn_last_sample = sample;
        btn_last_change_time = now;
        return;
    }

    // Debounced state after DEBOUNCE_MS
    if (absolute_time_diff_us(btn_last_change_time, now) >= (DEBOUNCE_MS * 1000)) {
        if (btn_stable_level != sample) {
            // Debounced edge occurred
            btn_stable_level = sample;

            if (sample == 0) {
                // Pressed (active LOW): start timing and clear long flag
                btn_pressed = true;
                press_start_time = now;
                long_fired = false;
            } else {
                // Released
                if (btn_pressed) {
                    btn_pressed = false;
                    // If long already fired during hold -> do nothing on release
                    if (!long_fired) {
                        int64_t press_ms = absolute_time_diff_us(press_start_time, now) / 1000;
                        if (press_ms >= LONG_PRESS_MS) {
                            // Edge case: exactly fired at release threshold (rare)
                            long_press_action();
                            long_fired = true;
                        } else {
                            next_state_short_press();
                        }
                    }
                }
            }
        }
    }

    // ---- Immediate long-press handling while held ----
    if (btn_pressed && !long_fired) {
        int64_t held_ms = absolute_time_diff_us(press_start_time, now) / 1000;
        if (held_ms >= LONG_PRESS_MS) {
            long_press_action();  // trigger immediately at 3s
            long_fired = true;    // prevent further actions until release
        }
    }
}

int main(void) {
    stdio_init_all(); // UART stdio enabled in CMake for Wokwi

    // --- PWM init for LED_PIN ---
    gpio_set_function(LED_PIN, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(LED_PIN);
    pwm_set_wrap(slice, PWM_WRAP);
    pwm_set_clkdiv(slice, PWM_CLKDIV);
    pwm_set_enabled(slice, true);

    apply_state(STATE_OFF);
    button_init();

    const int64_t half_period_ms = 1000 / (BLINK_HZ * 2);

    while (true) {
        button_poll();

        // Software blink driven by main loop (robust in Wokwi)
        if (current_state == STATE_BLINK) {
            if (time_reached(next_blink_toggle)) {
                blink_is_on = !blink_is_on;
                led_set_duty(blink_is_on ? DUTY_ON : DUTY_OFF);
                next_blink_toggle = make_timeout_time_ms(half_period_ms);
            }
        }

        sleep_ms(1);
    }
    return 0;
}
