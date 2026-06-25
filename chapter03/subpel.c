/*
 * subpel.c
 * Sub-pixel (half-pel and quarter-pel) interpolation for fractional
 * motion estimation.
 *
 * Companion code for Chapter 3: Motion Estimation.
 * Principles of Video Coding (Volume 2).
 *
 * Objects rarely move by a whole number of pixels.  The best match often
 * lies BETWEEN integer samples.  Here we manufacture those samples by
 * bilinear interpolation -- the simple two-tap average used by early
 * codecs.  Real standards use longer filters (H.264 six-tap, HEVC
 * eight-tap), but the principle is the same: invent the missing samples,
 * then match against them.
 */

#include <stdio.h>

/* Half-pel sample midway between two horizontal neighbours. */
double half_pel(int x1, int x2)
{
    return (x1 + x2) / 2.0;
}

/* Quarter-pel sample at fraction f (0, 0.25, 0.5, 0.75) between x1 and x2,
 * by linear interpolation. */
double quarter_pel(int x1, int x2, double f)
{
    return x1 + (x2 - x1) * f;
}

/* Bilinear interpolation at a fractional position (y + fy, x + fx) inside
 * an image of width w, height h (row-major).  fx, fy in [0, 1). */
double bilinear(const int *img, int w, int h, int y, int x,
                double fy, double fx)
{
    int y1 = (y + 1 < h) ? y + 1 : y;
    int x1 = (x + 1 < w) ? x + 1 : x;
    double a = img[y  * w + x ] * (1 - fx) + img[y  * w + x1] * fx;
    double b = img[y1 * w + x ] * (1 - fx) + img[y1 * w + x1] * fx;
    return a * (1 - fy) + b * fy;
}

int main(void)
{
    /* Worked Example 3.4: two neighbours 80 and 86. */
    printf("half-pel(80,86)    = %.2f\n", half_pel(80, 86));    /* 83.00 */
    printf("quarter-pel @0.25  = %.2f\n", quarter_pel(80, 86, 0.25));
    printf("quarter-pel @0.75  = %.2f\n", quarter_pel(80, 86, 0.75));

    {
        int img[4] = { 80, 86, 90, 100 };  /* 2x2 image, row-major */
        printf("bilinear @(0.5,0.5) = %.2f\n",
               bilinear(img, 2, 2, 0, 0, 0.5, 0.5));
    }
    return 0;
}
