/*
 * dpb_simulation.c
 * Principles of Video Coding -- Chapter 4
 * A small decoded picture buffer (DPB).  Pictures are marked short-term or
 * long-term; short-term references are retired as the buffer fills, while a
 * long-term reference is pinned.  The buffer state is printed each step.
 */
#include <stdio.h>

#define CAP 4

typedef enum { UNUSED, SHORT_TERM, LONG_TERM } RefKind;

typedef struct {
    int poc;
    RefKind kind;
} Picture;

static Picture dpb[CAP];

static void dpb_init(void){
    for (int i = 0; i < CAP; ++i){ dpb[i].kind = UNUSED; dpb[i].poc = -1; }
}

/* Insert a new picture, retiring the oldest short-term entry if needed. */
static void dpb_insert(int poc, RefKind kind){
    for (int i = 0; i < CAP; ++i)
        if (dpb[i].kind == UNUSED){ dpb[i].poc = poc; dpb[i].kind = kind; return; }
    int victim = -1, oldest = 1<<30;
    for (int i = 0; i < CAP; ++i)
        if (dpb[i].kind == SHORT_TERM && dpb[i].poc < oldest){ oldest = dpb[i].poc; victim = i; }
    if (victim >= 0){ dpb[victim].poc = poc; dpb[victim].kind = kind; }
}

static void dpb_print(int step){
    printf("step %d: [", step);
    for (int i = 0; i < CAP; ++i){
        const char *k = dpb[i].kind == UNUSED ? "-" :
                        dpb[i].kind == SHORT_TERM ? "S" : "L";
        if (dpb[i].kind == UNUSED) printf(" (--)");
        else printf(" (%d,%s)", dpb[i].poc, k);
    }
    printf(" ]\n");
}

int main(void){
    dpb_init();
    dpb_insert(0, LONG_TERM);   /* a pinned clean background */
    dpb_print(0);
    for (int f = 1; f <= 6; ++f){
        dpb_insert(f, SHORT_TERM);
        dpb_print(f);
    }
    printf("\nlong-term reference (poc 0) survives; short-term ones rotate out.\n");
    return 0;
}
