/*
 * benchmark_search.c
 * Compare full search, diamond search, and hexagon search on the same
 * data: both the motion vector found and the work done.
 *
 * Companion code for Chapter 3: Motion Estimation.
 * Principles of Video Coding (Volume 2).
 *
 * Rather than wall-clock time (which is noisy), we count the number of
 * SAD evaluations each algorithm performs -- a clean, machine-independent
 * proxy for cost.  Full search is optimal but expensive; the adaptive
 * searches reach the same (or nearly the same) vector with a tiny fraction
 * of the evaluations.
 */

#include <stdio.h>
#include <limits.h>

#define RW 64
#define RH 64
#define BS 16
#define P 16

static int ref[RH][RW];
static int cur[BS][BS];
static long sad_calls;          /* counts SAD evaluations */

static long sad_at(int ry, int rx)
{
    long sad = 0; int i, j;
    sad_calls++;
    if (ry < 0 || rx < 0 || ry + BS > RH || rx + BS > RW) return LONG_MAX;
    for (i = 0; i < BS; i++)
        for (j = 0; j < BS; j++) {
            int d = cur[i][j] - ref[ry + i][rx + j];
            sad += (d < 0) ? -d : d;
        }
    return sad;
}

static long full_search(int by, int bx, int *mvy, int *mvx)
{
    long best = LONG_MAX; int dy, dx; *mvy = 0; *mvx = 0;
    for (dy = -P; dy <= P; dy++)
        for (dx = -P; dx <= P; dx++) {
            long s = sad_at(by + dy, bx + dx);
            if (s < best) { best = s; *mvy = dy; *mvx = dx; }
        }
    return best;
}

static const int ldp[8][2] = {{0,2},{2,0},{0,-2},{-2,0},{1,1},{1,-1},{-1,1},{-1,-1}};
static const int sdp[4][2] = {{0,1},{1,0},{0,-1},{-1,0}};

static long diamond_search(int by, int bx, int *mvy, int *mvx)
{
    int cy = 0, cx = 0, k; long best = sad_at(by, bx);
    for (;;) {
        int ny = cy, nx = cx; long lb = best;
        for (k = 0; k < 8; k++) {
            long s = sad_at(by + cy + ldp[k][1], bx + cx + ldp[k][0]);
            if (s < lb) { lb = s; ny = cy + ldp[k][1]; nx = cx + ldp[k][0]; }
        }
        if (ny == cy && nx == cx) break;
        best = lb; cy = ny; cx = nx;
    }
    { int ny = cy, nx = cx; long lb = best;
      for (k = 0; k < 4; k++) {
          long s = sad_at(by + cy + sdp[k][1], bx + cx + sdp[k][0]);
          if (s < lb) { lb = s; ny = cy + sdp[k][1]; nx = cx + sdp[k][0]; }
      }
      best = lb; cy = ny; cx = nx; }
    *mvy = cy; *mvx = cx; return best;
}

static const int hex6[6][2] = {{2,0},{1,2},{-1,2},{-2,0},{-1,-2},{1,-2}};

static long hexagon_search(int by, int bx, int *mvy, int *mvx)
{
    int cy = 0, cx = 0, k; long best = sad_at(by, bx);
    for (;;) {
        int ny = cy, nx = cx; long lb = best;
        for (k = 0; k < 6; k++) {
            long s = sad_at(by + cy + hex6[k][1], bx + cx + hex6[k][0]);
            if (s < lb) { lb = s; ny = cy + hex6[k][1]; nx = cx + hex6[k][0]; }
        }
        if (ny == cy && nx == cx) break;
        best = lb; cy = ny; cx = nx;
    }
    { int ny = cy, nx = cx; long lb = best;
      for (k = 0; k < 4; k++) {
          long s = sad_at(by + cy + sdp[k][1], bx + cx + sdp[k][0]);
          if (s < lb) { lb = s; ny = cy + sdp[k][1]; nx = cx + sdp[k][0]; }
      }
      best = lb; cy = ny; cx = nx; }
    *mvy = cy; *mvx = cx; return best;
}

static void run(const char *name, long (*fn)(int,int,int*,int*))
{
    int mvy, mvx; long best;
    sad_calls = 0;
    best = fn(24, 24, &mvy, &mvx);
    printf("%-10s MV=(dx=%3d, dy=%3d)  SAD=%6ld  SAD calls=%ld\n",
           name, mvx, mvy, best, sad_calls);
}

int main(void)
{
    int i, j;
    for (i = 0; i < RH; i++)
        for (j = 0; j < RW; j++)
            ref[i][j] = (i * 7 + j * 3) & 0xFF;
    for (i = 0; i < BS; i++)
        for (j = 0; j < BS; j++)
            cur[i][j] = ref[24 + 4 + i][24 - 3 + j];  /* true MV (dx=-3, dy=4) */

    run("full",    full_search);
    run("diamond", diamond_search);
    run("hexagon", hexagon_search);
    return 0;
}
