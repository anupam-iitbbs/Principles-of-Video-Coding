/*
 * psnr_deblock.c  --  Measure PSNR improvement from deblocking.
 *
 * Chapter 8: In-Loop Filtering (Principles of Video Coding, Volume 2)
 *
 * We simulate the whole story on a synthetic image: an original smooth
 * ramp, a "quantized" version with block-boundary jumps, and a deblocked
 * version. PSNR is measured against the original before and after
 * filtering, so the improvement is a single number.
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define W 32
#define H 32
#define BS 8

static int clampi(int v,int lo,int hi){ return v<lo?lo:(v>hi?hi:v); }

static double psnr(int a[H][W],int b[H][W]){
    double mse=0.0;
    for(int y=0;y<H;y++)
        for(int x=0;x<W;x++){
            double d=a[y][x]-b[y][x];
            mse+=d*d;
        }
    mse/=(double)(W*H);
    if(mse==0.0) return 99.0;
    return 10.0*log10((255.0*255.0)/mse);
}

int main(void){
    static int orig[H][W],quant[H][W],deb[H][W];
    for(int y=0;y<H;y++)
        for(int x=0;x<W;x++)
            orig[y][x]=clampi(30 + 5*x,0,255);   /* smooth horizontal ramp */

    /* quantize: snap each block to its block-mean, creating seams */
    for(int by=0;by<H;by+=BS)
        for(int bx=0;bx<W;bx+=BS){
            long sum=0;
            for(int y=by;y<by+BS;y++) for(int x=bx;x<bx+BS;x++) sum+=orig[y][x];
            int mean=(int)(sum/(BS*BS));
            for(int y=by;y<by+BS;y++) for(int x=bx;x<bx+BS;x++) quant[y][x]=mean;
        }

    for(int y=0;y<H;y++) for(int x=0;x<W;x++) deb[y][x]=quant[y][x];
    /* deblock vertical seams of the copy */
    for(int y=0;y<H;y++)
        for(int bx=BS;bx<W;bx+=BS){
            int p0=deb[y][bx-1],q0=deb[y][bx],d=(q0-p0)/4;
            d=clampi(d,-16,16);
            deb[y][bx-1]=clampi(p0+d,0,255);
            deb[y][bx]=clampi(q0-d,0,255);
        }

    printf("PSNR before deblocking: %.2f dB\n",psnr(orig,quant));
    printf("PSNR after  deblocking: %.2f dB\n",psnr(orig,deb));
    return 0;
}
