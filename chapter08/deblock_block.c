/*
 * deblock_block.c  --  Deblocking a small image tiled into 8x8 blocks.
 *
 * Chapter 8: In-Loop Filtering (Principles of Video Coding, Volume 2)
 *
 * Extends the 1D idea to two dimensions: we filter both vertical and
 * horizontal block boundaries of a WxH grayscale image whose samples are
 * organised into 8x8 blocks. Vertical boundaries are filtered first, then
 * horizontal, mirroring the ordering real codecs impose for bit-exactness.
 */
#include <stdio.h>
#include <stdlib.h>

#define W 16
#define H 16
#define BS 8   /* block size */

static int clampi(int v,int lo,int hi){ return v<lo?lo:(v>hi?hi:v); }

static void filter_pair(int *a,int *b,int alpha,int beta,int inner){
    int jump=abs(*a-*b);
    if(jump<alpha && inner<beta){
        int delta=(*b-*a)/4;
        delta=clampi(delta,-(jump/2),(jump/2));
        *a=clampi(*a+delta,0,255);
        *b=clampi(*b-delta,0,255);
    }
}

int main(void){
    static int img[H][W];
    /* build an image: flat left half, flat-but-brighter right half */
    for(int y=0;y<H;y++)
        for(int x=0;x<W;x++)
            img[y][x]=(x<BS)?64:80;

    int alpha=20, beta=8;

    /* vertical block boundaries (columns that are multiples of BS) */
    for(int y=0;y<H;y++){
        for(int bx=BS;bx<W;bx+=BS){
            int inner=abs(img[y][bx-2]-img[y][bx-1])+abs(img[y][bx+1]-img[y][bx]);
            filter_pair(&img[y][bx-1],&img[y][bx],alpha,beta,inner);
        }
    }
    /* horizontal block boundaries (rows that are multiples of BS) */
    for(int x=0;x<W;x++){
        for(int by=BS;by<H;by+=BS){
            int inner=abs(img[by-2][x]-img[by-1][x])+abs(img[by+1][x]-img[by][x]);
            filter_pair(&img[by-1][x],&img[by][x],alpha,beta,inner);
        }
    }

    /* print one representative row crossing the vertical boundary */
    printf("row 0 after deblocking:");
    for(int x=0;x<W;x++) printf(" %d",img[0][x]);
    printf("\n");
    return 0;
}
