#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt
import os

def bench(algo, n, pat):
    binary = 'build/treeint'

    times = os.popen(f"taskset -c 1 ./{binary} {algo} {n}").read()
    # strip out the '\n'
    times = times.strip()
    times = [int(x) for x in times.split(',')]

    return times[pat]

# make sure we do make before everything start
os.system("make")

# 0: insert, 1: find, 2: remove
pat = 0

algo_list=["s-tree"]
nsize = list(int(2**k) for k in range(7, 20))
ts = [[bench(algo, size, pat) for size in nsize] for algo in algo_list]

fig, ax = plt.subplots()
for idx, t in enumerate(ts):
    ax.plot(nsize, t, label=algo_list[idx])
ax.set_xlabel('Tree size')
ax.set_xscale('log', base=2)
ax.set_ylabel('Latency(ns)')
ax.set_ylim(bottom=0, top=None)
ax.legend()
plt.show()
