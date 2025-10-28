#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "buttonkit.h"

#define LED_PIN 15
#define BUTTON_PIN 14

#define PWM_WRAP 1000
#define PWM_CLKDIV 125
#define BLINK_HZ 2
#define DUTY_OFF 0
#define DUTY_ON PWM_WRAP
#define DUTY_DIM (PWM_WRAP / 4)

typedef enum {
    STATE_OFF = 0,
    STATE_ON,
    STATE_DIM_25,
    STATE_BLINK
} led_state_t;

static led_state_t current_state = STATE_OFF;
static led_state_t last_non_off_state = STATE_ON;
static bool off_memory_mode = false;
static absolute_time_t next_blink_toggle;
static bool blink_is_on = false;

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
        case STATE_BLINK:
            blink_is_on = false;
            next_blink_toggle = make_timeout_time_ms(1000 / (BLINK_HZ * 2));
            led_set_duty(DUTY_OFF);
            last_non_off_state = STATE_BLINK;
            break;
    }
}

static void next_state_short_press(void) {
    switch (current_state) {
        case STATE_OFF:
            apply_state(off_memory_mode ? last_non_off_state : STATE_ON);
            off_memory_mode = false;
            break;
        case STATE_ON: apply_state(STATE_DIM_25); break;
        case STATE_DIM_25: apply_state(STATE_BLINK); break;
        case STATE_BLINK:
            apply_state(STATE_OFF);
            off_memory_mode = false;
            break;
    }
}

static void long_press_action(void) {
    off_memory_mode = true;
    apply_state(STATE_OFF);
}

int main(void) {
    stdio_init_all();

    gpio_set_function(LED_PIN, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(LED_PIN);
    pwm_set_wrap(slice, PWM_WRAP);
    pwm_set_clkdiv(slice, PWM_CLKDIV);
    pwm_set_enabled(slice, true);

    apply_state(STATE_OFF);
    buttonkit_init(BUTTON_PIN);

    const int64_t half_period_ms = 1000 / (BLINK_HZ * 2);

    while (true) {
        buttonkit_poll();

        if (buttonkit_was_short_pressed()) next_state_short_press();
        if (buttonkit_was_long_pressed()) long_press_action();

        if (current_state == STATE_BLINK && time_reached(next_blink_toggle)) {
            blink_is_on = !blink_is_on;
            led_set_duty(blink_is_on ? DUTY_ON : DUTY_OFF);
            next_blink_toggle = make_timeout_time_ms(half_period_ms);
        }

        sleep_ms(1);
    }
}
