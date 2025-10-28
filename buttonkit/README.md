# ButtonKit Library

## Overview
**ButtonKit** is a lightweight library for handling push buttons on the Raspberry Pi Pico.  
It provides simple initialization and polling functions that automatically handle
debouncing and detection of short and long presses.  

The goal of this library is to simplify button input logic by encapsulating
the timing and filtering details into a reusable component.

---

## Features
- Configurable GPIO pin for the button input  
- Automatic **debounce** filtering (default: 30 ms)
- Detection of **short press** and **long press** events (default long press: 3000 ms)
- Simple polling interface (no interrupts required)
- Designed for easy integration into small Pico projects

---

## API Reference

### `void buttonkit_init(uint pin)`
Initializes the library and sets up the given GPIO pin as a pulled-up input.  
Call this once before using any other functions.

| Parameter | Description |
|------------|-------------|
| `pin` | The GPIO pin number where the button is connected. |

---

### `bool buttonkit_poll(void)`
Must be called periodically (e.g. inside the main loop).  
Handles debouncing, state tracking, and event timing.  

| Returns | Meaning |
|----------|----------|
| `true` | The button is currently pressed (logic low). |
| `false` | The button is released. |

---

### `bool buttonkit_was_short_pressed(void)`
Returns `true` for one poll cycle after a valid **short press** is detected.  
Use this to trigger short-press actions.

---

### `bool buttonkit_was_long_pressed(void)`
Returns `true` for one poll cycle after a **long press** is detected.  
Useful for alternate button functions such as "reset" or "mode switch".

---

## Example Usage

```c
#include "pico/stdlib.h"
#include "buttonkit.h"

#define BUTTON_PIN 15
#define LED_PIN    25

int main() {
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    buttonkit_init(BUTTON_PIN);

    while (true) {
        buttonkit_poll();

        if (buttonkit_was_short_pressed()) {
            gpio_xor_mask(1u << LED_PIN);  // Toggle LED on short press
        }

        if (buttonkit_was_long_pressed()) {
            gpio_put(LED_PIN, 0);           // Turn LED off on long press
        }

        sleep_ms(10);
    }
}
