/*
 * visualize_blocking.c  --  Highlight block boundaries before/after filtering.
 *
 * Chapter 8: In-Loop Filtering (Principles of Video Coding, Volume 2)
 *
 * Writes two PGM (P2, ASCII) images: one where the seams between 8x8 blocks
 * are emphasised, and one after a simple deblocking pass. Open them in any
 * image viewer to see the blocking effect disappear. PGM keeps the program
 * dependency-free.
 */
#include <stdio.h>
#include <stdlib.h>

#define W 32
#define H 32
#define BS 8

static int clampi(int v,int lo,int hi){ return v<lo?lo:(v>hi?hi:v); }

static void write_pgm(const char *fn,int img[H][W]){
    FILE *f=fopen(fn,"w");
    if(!f){ perror(fn); return; }
    fprintf(f,"P2\n%d %d\n255\n",W,H);
    for(int y=0;y<H;y++){
        for(int x=0;x<W;x++) fprintf(f,"%d ",img[y][x]);
        fprintf(f,"\n");
    }
    fclose(f);
}

int main(void){
    static int img[H][W];
    /* per-block flat levels forming a staircase -> strong seams */
    for(int y=0;y<H;y++)
        for(int x=0;x<W;x++)
            img[y][x]=40 + 30*((x/BS)+(y/BS));

    write_pgm("blocking_before.pgm",img);

    /* deblock vertical then horizontal seams */
    for(int y=0;y<H;y++)
        for(int bx=BS;bx<W;bx+=BS){
            int p0=img[y][bx-1],q0=img[y][bx],d=(q0-p0)/4;
            d=clampi(d,-8,8);
            img[y][bx-1]=clampi(p0+d,0,255);
            img[y][bx]=clampi(q0-d,0,255);
        }
    for(int x=0;x<W;x++)
        for(int by=BS;by<H;by+=BS){
            int p0=img[by-1][x],q0=img[by][x],d=(q0-p0)/4;
            d=clampi(d,-8,8);
            img[by-1][x]=clampi(p0+d,0,255);
            img[by][x]=clampi(q0-d,0,255);
        }

    write_pgm("blocking_after.pgm",img);
    printf("wrote blocking_before.pgm and blocking_after.pgm\n");
    return 0;
}
