/*
 * bit_budget.c  --  Companion code for Chapter 6: Rate Control
 * Principles of Video Coding
 *
 * Computes the average per-frame bit budget from a target bitrate
 * and frame rate, and shows how the budget scales with resolution.
 *
 *   Bits per frame = Bitrate / FPS        (Equation 6.2)
 *
 * Build:  gcc -O2 -o bit_budget bit_budget.c -lm
 * Run:    ./bit_budget
 */

#include <stdio.h>

/* Average bits available to a single frame. */
static double bits_per_frame(double bitrate_bps, double fps)
{
    return bitrate_bps / fps;
}

/* Bits available per pixel, a rough difficulty-independent measure. */
static double bits_per_pixel(double bitrate_bps, double fps, long pixels)
{
    return bits_per_frame(bitrate_bps, fps) / (double)pixels;
}

int main(void)
{
    double bitrate = 2.0e6;   /* 2 Mbps target              */
    double fps     = 30.0;    /* 30 frames per second       */

    struct { const char *name; long w, h; } modes[] = {
        { "480p",  854,  480 },
        { "720p",  1280, 720 },
        { "1080p", 1920, 1080 },
        { "4K",    3840, 2160 },
    };
    int n = (int)(sizeof(modes) / sizeof(modes[0]));

    double bpf = bits_per_frame(bitrate, fps);
    printf("Target bitrate : %.0f bits/s\n", bitrate);
    printf("Frame rate     : %.0f fps\n", fps);
    printf("Bits per frame : %.0f bits (%.2f kbit)\n\n", bpf, bpf / 1000.0);

    printf("%-8s %12s %14s\n", "Mode", "Pixels", "Bits/pixel");
    for (int i = 0; i < n; ++i) {
        long px = modes[i].w * modes[i].h;
        double bpp = bits_per_pixel(bitrate, fps, px);
        printf("%-8s %12ld %14.4f\n", modes[i].name, px, bpp);
    }

    printf("\nNote: the per-frame budget is fixed by bitrate and fps,\n");
    printf("so higher resolutions get far fewer bits per pixel.\n");
    return 0;
}
