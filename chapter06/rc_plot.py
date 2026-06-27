#!/usr/bin/env python3
"""
rc_plot.py  --  Companion code for Chapter 6: Rate Control
Principles of Video Coding

Simulates a one-pass rate controller over a sequence of frames and
plots three signals that tell the whole story of rate control:
  - the per-frame bitrate (versus the target),
  - the quantization parameter the controller chose, and
  - the buffer fullness over time.

The point is to *see* the controller breathe: when bits overshoot, QP
rises and the buffer drains; when bits undershoot, QP falls and the
buffer fills back up.

Run:  python3 rc_plot.py
Needs:  numpy, matplotlib
"""

import numpy as np
import matplotlib.pyplot as plt


def model_bits(complexity, qp, scale=600000.0):
    """Bits for a frame: +6 in QP roughly halves the bits."""
    return complexity * scale * 2.0 ** (-qp / 6.0)


def simulate(n=60, target=66667.0, cap=200000.0, drain=66667.0):
    rng = np.random.default_rng(0)
    qp = 30.0
    level = cap / 2.0

    bits_hist, qp_hist, buf_hist = [], [], []

    for i in range(n):
        # A complexity signal with a couple of hard scenes.
        complexity = 1.0 + 0.4 * np.sin(i * 0.3)
        if 20 <= i < 28 or 40 <= i < 45:
            complexity += 1.5
        complexity += 0.1 * rng.standard_normal()
        complexity = max(0.2, complexity)

        bits = model_bits(complexity, qp)

        # Update the buffer: fill with produced bits, drain steadily.
        level = min(cap, level + bits)
        level = max(0.0, level - drain)

        bits_hist.append(bits)
        qp_hist.append(qp)
        buf_hist.append(level)

        # Proportional QP update on the log-ratio of bits to target.
        err = np.log2(bits / target)
        qp = float(np.clip(qp + 3.0 * err, 10.0, 51.0))

    return (np.array(bits_hist), np.array(qp_hist),
            np.array(buf_hist), target, cap)


def main():
    bits, qp, buf, target, cap = simulate()
    t = np.arange(len(bits))

    fig, ax = plt.subplots(3, 1, figsize=(8, 7), sharex=True)

    ax[0].plot(t, bits / 1000.0, label="bits/frame")
    ax[0].axhline(target / 1000.0, color="r", ls="--", label="target")
    ax[0].set_ylabel("kbit")
    ax[0].legend(loc="upper right")
    ax[0].set_title("Rate control over time")

    ax[1].plot(t, qp, color="green")
    ax[1].set_ylabel("QP")

    ax[2].plot(t, buf / 1000.0, color="purple")
    ax[2].axhline(cap / 1000.0, color="r", ls=":", label="capacity")
    ax[2].set_ylabel("buffer (kbit)")
    ax[2].set_xlabel("frame")
    ax[2].legend(loc="upper right")

    fig.tight_layout()
    plt.savefig("rc_plot.png", dpi=120)
    print("Saved rc_plot.png")


if __name__ == "__main__":
    main()
