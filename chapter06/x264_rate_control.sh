#!/usr/bin/env bash
#
# x264_rate_control.sh  --  Companion script for Chapter 6: Rate Control
# Principles of Video Coding
#
# Demonstrates the three rate-control flags discussed in the chapter,
# using ffmpeg with libx264. Run each block and compare the output
# size and quality to build intuition for how the VBV constraints bite.
#
# Requires: ffmpeg built with --enable-libx264
# Usage:    ./x264_rate_control.sh input.mp4

set -euo pipefail

INPUT="${1:-input.mp4}"

# ---------------------------------------------------------------------
# 1) Constrained ABR: average 4 Mbps, capped at 4 Mbps, 8 Mbit VBV.
#
#    -b:v 4M       target (average) bitrate
#    -maxrate 4M   maximum instantaneous bitrate (the VBV drain rate)
#    -bufsize 8M   VBV buffer size; larger = smoother but more latency
# ---------------------------------------------------------------------
ffmpeg -i "$INPUT" \
       -c:v libx264 \
       -b:v 4M \
       -maxrate 4M \
       -bufsize 8M \
       out_cbr.mp4

# ---------------------------------------------------------------------
# 2) Constant quality (CRF): no bitrate target at all.
#    The encoder chooses the bitrate to hold quality fixed.
#    Try 18 (near-lossless), 23 (default), 28 (low bitrate).
# ---------------------------------------------------------------------
ffmpeg -i "$INPUT" -c:v libx264 -crf 23 out_crf23.mp4

# ---------------------------------------------------------------------
# 3) Two-pass VBR: first pass measures complexity, second allocates.
#    Best quality at a fixed average bitrate for offline encodes.
# ---------------------------------------------------------------------
ffmpeg -y -i "$INPUT" -c:v libx264 -b:v 4M -pass 1 -f null /dev/null
ffmpeg    -i "$INPUT" -c:v libx264 -b:v 4M -pass 2 out_2pass.mp4

echo "Done. Compare sizes:"
ls -lh out_cbr.mp4 out_crf23.mp4 out_2pass.mp4
