/*
 * reference_buffer.c
 * Principles of Video Coding -- Chapter 4
 * A simple ring of reference frames that future frames can index for
 * prediction.  New decoded frames push out the oldest reference once the
 * buffer is full, modelling the most basic reference picture list.
 */
#include <stdio.h>

#define CAP 4   /* buffer capacity */

typedef struct {
    int poc[CAP];   /* picture order count of each held reference */
    int count;      /* number currently valid */
    int head;       /* index of newest entry */
} RefBuffer;

static void rb_init(RefBuffer *b){ b->count = 0; b->head = 0; }

static void rb_push(RefBuffer *b, int poc){
    b->head = (b->head + 1) % CAP;
    b->poc[b->head] = poc;
    if (b->count < CAP) b->count++;
}

/* index 0 = newest reference, 1 = next newest, ... */
static int rb_get(const RefBuffer *b, int idx){
    int pos = (b->head - idx + CAP) % CAP;
    return b->poc[pos];
}

int main(void){
    RefBuffer b; rb_init(&b);
    for (int frame = 0; frame < 7; ++frame){
        rb_push(&b, frame);
        printf("after decoding frame %d, references (newest first):", frame);
        for (int i = 0; i < b.count; ++i) printf(" %d", rb_get(&b, i));
        printf("\n");
    }
    return 0;
}
