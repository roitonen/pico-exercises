#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define LINE_MAX 128
#define HIST_MAX 10

typedef struct {
    int a, b;
    char op;
    char *expr; // e.g. "6 + 6"
} HistEntry;

typedef struct {
    HistEntry **items;
    int count;
} History;

static void trim(char *s) {
    size_t len = strlen(s);
    while (len && (s[len - 1] == '\n' || s[len - 1] == '\r' || isspace((unsigned char)s[len - 1])))
        s[--len] = '\0';
    size_t i = 0;
    while (isspace((unsigned char)s[i])) i++;
    if (i) memmove(s, s + i, strlen(s + i) + 1);
}

static int compute(int a, char op, int b, int *out, const char **errmsg) {
    switch (op) {
        case '+': *out = a + b; return 1;
        case '-': *out = a - b; return 1;
        case '*': *out = a * b; return 1;
        case '/':
            if (b == 0) { if (errmsg) *errmsg = "Virhe: nollalla jakaminen ei ole sallittua."; return 0; }
            *out = a / b; return 1;
        case '%':
            if (b == 0) { if (errmsg) *errmsg = "Virhe: nollalla jakojäännös ei ole sallittua."; return 0; }
            *out = a % b; return 1;
        default:
            if (errmsg) *errmsg = "Virheellinen syöte (puuttuvia argumentteja)";
            return 0;
    }
}

static void history_init(History *h) {
    h->items = (HistEntry**)malloc(sizeof(HistEntry*) * HIST_MAX);
    h->count = 0;
}

static void history_free_all(History *h) {
    if (!h || !h->items) return;
    for (int i = 0; i < h->count; ++i) {
        free(h->items[i]->expr);
        free(h->items[i]);
    }
    free(h->items);
    h->items = NULL;
    h->count = 0;
}

static void history_clear(History *h) {
    for (int i = 0; i < h->count; ++i) {
        free(h->items[i]->expr);
        free(h->items[i]);
    }
    h->count = 0;
}

static void history_add(History *h, int a, char op, int b) {
    HistEntry *e = (HistEntry*)malloc(sizeof(HistEntry));
    if (!e) return;
    e->a = a; e->b = b; e->op = op;

    char buf[64];
    snprintf(buf, sizeof(buf), "%d %c %d", a, op, b);
    e->expr = strdup(buf);
    if (!e->expr) { free(e); return; }

    if (h->count == HIST_MAX) {
        free(h->items[0]->expr);
        free(h->items[0]);
        memmove(&h->items[0], &h->items[1], sizeof(HistEntry*) * (HIST_MAX - 1));
        h->items[HIST_MAX - 1] = e;
    } else {
        h->items[h->count++] = e;
    }
}

static void history_print(const History *h) {
    if (h->count == 0) {
        printf("Komentohistoria on tyhjä.\n");
        return;
    }
    printf("Komentohistoria:\n");
    for (int i = 0; i < h->count; ++i) {
        printf("%d: %s\n", i + 1, h->items[i]->expr);
    }
}

static int history_replay(const History *h, int idx) {
    if (idx < 1 || idx > h->count) {
        printf("Virhe: historiaviitettä ei löydy: !%d\n", idx);
        return 0;
    }
    HistEntry *e = h->items[idx - 1];
    int result;
    const char *err = NULL;
    if (!compute(e->a, e->op, e->b, &result, &err)) {
        if (err) printf("%s\n", err);
        else printf("Virhe: historiassa oleva lasku ei ole suoritettavissa.\n");
        return 0;
    }
    printf("%s = %d\n", e->expr, result);
    return 1;
}

int main(void) {
    stdio_init_all();      // UART stdio enabled in CMake for Wokwi
    sleep_ms(100);         // let UART settle

    History hist;
    history_init(&hist);

    char line[LINE_MAX];

    while (true) {
        printf("> ");
        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin)) {
            printf("Virheellinen syöte (tyhjä) kirjoita \"q\" lopettaaksesi ohjelman\n");
            continue;
        }

        trim(line);
        if (line[0] == '\0') {
            printf("Virheellinen syöte (tyhjä) kirjoita \"q\" lopettaaksesi ohjelman\n");
            continue;
        }

        if (strcmp(line, "q") == 0) break;

        if (strcmp(line, "h") == 0) { history_print(&hist); continue; }
        if (strcmp(line, "c") == 0) { history_clear(&hist); continue; }

        if (line[0] == '!') {
            const char *p = line + 1;
            for (const char *q = p; *q; ++q)
                if (!isdigit((unsigned char)*q)) { printf("Virhe: historiaviite virheellinen\n"); p = NULL; break; }
            if (!p || !*p) continue;
            int idx = atoi(p);
            history_replay(&hist, idx);
            continue;
        }

        int a, b; char op;
        int matched = sscanf(line, "%d %c %d", &a, &op, &b);
        if (matched != 3 || (op!='+' && op!='-' && op!='*' && op!='/' && op!='%')) {
            printf("Virheellinen syöte (puuttuvia argumentteja)\n");
            continue;
        }

        int result; const char *errmsg = NULL;
        if (!compute(a, op, b, &result, &errmsg)) {
            if (errmsg) printf("%s\n", errmsg);
            else printf("Virheellinen syöte (puuttuvia argumentteja)\n");
            continue;
        }

        printf("%d\n", result);
        history_add(&hist, a, op, b);
    }

    history_free_all(&hist);
    sleep_ms(50);
    return 0;
}
