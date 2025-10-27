#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define LINE_MAX 128

static void trim(char *s) {
    size_t len = strlen(s);
    while (len && isspace((unsigned char)s[len - 1])) s[--len] = '\0';
    size_t i = 0;
    while (isspace((unsigned char)s[i])) i++;
    if (i) memmove(s, s + i, strlen(s + i) + 1);
}

int main(void) {
    stdio_init_all();
    char line[LINE_MAX];

    while (true) {
        printf("> "); fflush(stdout); 
        if (!fgets(line, sizeof(line), stdin)) {
            printf("input error\n");
            continue;
        }
        trim(line);
        if (!*line) { printf("Virheellinen syöte (tyhjä) kirjoita \"q\" lopettaaksesi ohjelman\n"); continue; }
        if (strcmp(line, "q") == 0) break;

        int a, b; char op;
        if (sscanf(line, "%d %c %d", &a, &op, &b) != 3) {
            printf("Virheellinen syöte (puuttuvia argumentteja)\n");
            continue;
        }
        switch (op) {
            case '+': printf("%d\n", a + b); break;
            case '-': printf("%d\n", a - b); break;
            case '*': printf("%d\n", a * b); break;
            case '/': if (b) printf("%d\n", a / b); else puts("Virhe: nollalla jakaminen ei ole sallittua."); break;
            case '%': if (b) printf("%d\n", a % b); else puts("Virhe: nollalla jakojäännös ei ole sallittua."); break;
            default:  puts("Virheellinen syöte (puuttuvia argumentteja)"); break;
        }
    }
    return 0;
}
