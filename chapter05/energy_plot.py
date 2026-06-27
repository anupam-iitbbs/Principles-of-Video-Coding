#!/usr/bin/env python3
"""
energy_plot.py
Companion code for Chapter 5: Transform Coding (Volume 2)

Plots the magnitude of DCT coefficients of an 8x8 block so the reader
can SEE energy compaction: a few large low-frequency coefficients and a
long tail of near-zero high-frequency coefficients. Requires numpy and
matplotlib (pip install numpy matplotlib).
"""

import numpy as np
import matplotlib.pyplot as plt

N = 8


def dct_matrix(n):
    """Return the orthonormal n x n DCT-II transform matrix T."""
    T = np.zeros((n, n))
    for u in range(n):
        alpha = np.sqrt(1.0 / n) if u == 0 else np.sqrt(2.0 / n)
        for i in range(n):
            T[u, i] = alpha * np.cos((2 * i + 1) * u * np.pi / (2 * n))
    return T


def main():
    # A smooth gradient block: a typical compressible residual.
    x = np.add.outer(np.arange(N), np.arange(N)) * 4.0 + 40.0

    T = dct_matrix(N)
    # Separable 2-D DCT:  X = T x T^T
    X = T @ x @ T.T

    mags = np.abs(X).flatten()
    order = np.argsort(mags)[::-1]          # largest first
    sorted_mags = mags[order]

    plt.figure(figsize=(7, 4))
    plt.bar(range(len(sorted_mags)), sorted_mags)
    plt.xlabel("coefficient (sorted by magnitude)")
    plt.ylabel("|coefficient|")
    plt.title("DCT energy compaction: a few coefficients dominate")
    plt.tight_layout()
    plt.savefig("energy_plot.png", dpi=150)
    print("Saved energy_plot.png")
    print("Largest coefficient:  %.2f" % sorted_mags[0])
    print("Smallest coefficient: %.4f" % sorted_mags[-1])


if __name__ == "__main__":
    main()
