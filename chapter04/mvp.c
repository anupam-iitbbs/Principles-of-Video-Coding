/*
 * mvp.c
 * Principles of Video Coding -- Chapter 4
 * Motion vector prediction.  Form a median predictor from the left, top and
 * top-right neighbour vectors, then code only the motion vector difference
 * MVD = MV - MVP.  Decoder reconstructs MV = MVP + MVD.
 */
#include <stdio.h>

typedef struct { int x, y; } MV;

static int median3(int a, int b, int c){
    if ((a >= b && a <= c) || (a <= b && a >= c)) return a;
    if ((b >= a && b <= c) || (b <= a && b >= c)) return b;
    return c;
}

static MV mvp(MV left, MV top, MV topright){
    MV p;
    p.x = median3(left.x, top.x, topright.x);
    p.y = median3(left.y, top.y, topright.y);
    return p;
}

int main(void){
    MV left = {4, 0}, top = {5, 1}, topright = {3, 0};
    MV truth = {4, 1};

    MV p = mvp(left, top, topright);
    MV mvd = { truth.x - p.x, truth.y - p.y };
    MV rec = { p.x + mvd.x, p.y + mvd.y };

    printf("neighbours: L(%d,%d) T(%d,%d) TR(%d,%d)\n",
           left.x, left.y, top.x, top.y, topright.x, topright.y);
    printf("MVP   = (%d,%d)\n", p.x, p.y);
    printf("MV    = (%d,%d)\n", truth.x, truth.y);
    printf("MVD   = (%d,%d)   <- only this is transmitted\n", mvd.x, mvd.y);
    printf("recon = (%d,%d)\n", rec.x, rec.y);
    return 0;
}
