#include <stdio.h>
#include "pico/stdlib.h"

int main() {
    stdio_init_all();
    sleep_ms(200);
    printf("Hello, Pico!\n");
    sleep_ms(100);
    return 0;
}
