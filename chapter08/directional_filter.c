/*
 * directional_filter.c  --  A CDEF-like directional filter.
 *
 * Chapter 8: In-Loop Filtering (Principles of Video Coding, Volume 2)
 *
 * CDEF first estimates the dominant edge direction of a block, then filters
 * ALONG that direction so ringing is suppressed without softening the edge.
 * We implement a tiny version: for an 8x8 block we test a few directions,
 * pick the one whose along-direction differences are smallest, and apply a
 * constrained 3-tap average along it.
 */
#include <stdio.h>
#include <stdlib.h>

#define BS 8

static int clampi(int v,int lo,int hi){ return v<lo?lo:(v>hi?hi:v); }

/* four coarse directions as (dx,dy) steps */
static const int DX[4]={1,1,0,-1};
static const int DY[4]={0,1,1,1};   /* horizontal, diag-down, vertical, diag-up */

static int in_range(int x,int y){ return x>=0&&x<BS&&y>=0&&y<BS; }

/* score a direction by summing |differences| ALONG it (low = edge runs this way) */
static long score_dir(int b[BS][BS],int d){
    long s=0;
    for(int y=0;y<BS;y++) for(int x=0;x<BS;x++){
        int nx=x+DX[d], ny=y+DY[d];
        if(in_range(nx,ny)) s+=abs(b[y][x]-b[ny][nx]);
    }
    return s;
}

int main(void){
    int b[BS][BS];
    /* a diagonal-down edge: dark above-left, bright below-right */
    for(int y=0;y<BS;y++) for(int x=0;x<BS;x++) b[y][x]=(x+y<BS)?40:200;

    int best=0; long bestscore=score_dir(b,0);
    for(int d=1;d<4;d++){ long sc=score_dir(b,d); if(sc<bestscore){bestscore=sc;best=d;} }
    const char *names[4]={"horizontal","diag-down","vertical","diag-up"};
    printf("chosen edge direction: %s\n", names[best]);

    /* constrained 3-tap average ALONG the chosen direction */
    int out[BS][BS];
    for(int y=0;y<BS;y++) for(int x=0;x<BS;x++){
        int c=b[y][x], acc=c, cnt=1;
        int fx=x+DX[best], fy=y+DY[best];
        int bx=x-DX[best], by=y-DY[best];
        if(in_range(fx,fy)){ acc+=clampi(b[fy][fx],c-16,c+16); cnt++; }
        if(in_range(bx,by)){ acc+=clampi(b[by][bx],c-16,c+16); cnt++; }
        out[y][x]=acc/cnt;
    }
    printf("center sample %d -> filtered %d\n", b[3][4], out[3][4]);
    return 0;
}
