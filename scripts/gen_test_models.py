#!/usr/bin/env python3
"""
Generates test models for DwRast.

usage: gen_test_models.py <out.obj> [<out.obj> ...]

The model is picked by the file name:
    torus.obj       ~2.3k triangles, quads with v/vt/vn and vertex colors
    cube.obj        8 vertexes, negative indexes, v//vn, CRLF line endings
    knot_100k.obj   ~100k triangles, (2,3) torus knot, lots of self overlap
    knot_400k.obj   ~400k triangles, same knot but denser
"""

import math
import os
import sys


def torus(f, R=1.0, r=0.4, N=48, M=24):
    f.write("# test torus\no torus\n")
    for i in range(N):
        for j in range(M):
            u, v = 2 * math.pi * i / N, 2 * math.pi * j / M
            x = (R + r * math.cos(v)) * math.cos(u)
            y = r * math.sin(v)
            z = (R + r * math.cos(v)) * math.sin(u)
            f.write(f"v {x:.5f} {y:.5f} {z:.5f} 0.95 {0.55 + 0.4 * i / N:.2f} 0.25\n")
            f.write(f"vt {i / N:.4f} {j / M:.4f}\n")
            f.write(f"vn {math.cos(v) * math.cos(u):.4f} {math.sin(v):.4f} {math.cos(v) * math.sin(u):.4f}\n")
    for i in range(N):
        for j in range(M):
            a = i * M + j + 1
            b = ((i + 1) % N) * M + j + 1
            c = ((i + 1) % N) * M + (j + 1) % M + 1
            d = i * M + (j + 1) % M + 1
            f.write(f"f {a}/{a}/{a} {b}/{b}/{b} {c}/{c}/{c} {d}/{d}/{d}\n")


def cube(f):
    f.write("# cube\r\nmtllib cube.mtl\r\n")
    for x in (-1, 1):
        for y in (-1, 1):
            for z in (-1, 1):
                f.write(f"  v {x} {y} {z}\r\n")
    f.write("vn 0 0 1\r\n")
    for face in [(1, 2, 4, 3), (5, 7, 8, 6), (1, 5, 6, 2), (3, 4, 8, 7), (1, 3, 7, 5), (2, 6, 8, 4)]:
        f.write("f " + " ".join(f"{k - 9}//-1" for k in face) + "  # quad\r\n")


def knot(f, N, M, p=2, q=3, r=0.35):
    def curve(t):
        return ((2 + math.cos(q * t)) * math.cos(p * t),
                (2 + math.cos(q * t)) * math.sin(p * t),
                math.sin(q * t))

    def normalize(v):
        length = math.sqrt(sum(x * x for x in v))
        return [x / length for x in v]

    def cross(a, b):
        return [a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]]

    f.write(f"# ({p},{q}) torus knot\no knot\n")
    for i in range(N):
        t = 2 * math.pi * i / N
        P, Q = curve(t), curve(t + 1e-4)
        T = normalize([Q[k] - P[k] for k in range(3)])
        B = normalize(cross(T, P))
        Nn = cross(B, T)

        h = i / N
        color = [0.5 + 0.5 * math.cos(2 * math.pi * (h - k / 3)) for k in range(3)]

        for j in range(M):
            a = 2 * math.pi * j / M
            x, y, z = (P[k] + r * (math.cos(a) * Nn[k] + math.sin(a) * B[k]) for k in range(3))
            f.write(f"v {x:.5f} {y:.5f} {z:.5f} {color[0]:.3f} {color[1]:.3f} {color[2]:.3f}\n")

    for i in range(N):
        for j in range(M):
            a = i * M + j + 1
            b = ((i + 1) % N) * M + j + 1
            c = ((i + 1) % N) * M + (j + 1) % M + 1
            d = i * M + (j + 1) % M + 1
            f.write(f"f {a} {b} {c} {d}\n")


MODELS = {
    "torus.obj": torus,
    "cube.obj": cube,
    "knot_100k.obj": lambda f: knot(f, 800, 64),
    "knot_400k.obj": lambda f: knot(f, 1600, 128),
}


def main():
    if len(sys.argv) < 2:
        print(__doc__.strip())
        return 1

    for path in sys.argv[1:]:
        name = os.path.basename(path)
        if name not in MODELS:
            print(f"[ERROR] unknown model {name}, known: {', '.join(MODELS)}")
            return 1

        os.makedirs(os.path.dirname(path) or ".", exist_ok=True)
        # newline="" so the cube keeps its CRLF line endings
        with open(path, "w", newline="") as f:
            MODELS[name](f)
        print(f"[info] generated {path}")

    return 0


if __name__ == "__main__":
    sys.exit(main())
