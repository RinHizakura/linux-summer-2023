#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt
import os

def stat(data):
    mean, std = data.mean(), data.std()
    # identify outliers: 2 std = 95%
    cut_off = std * 2
    lower, upper = mean - cut_off, mean + cut_off
    data = data[data > lower]
    data = data[data < upper]

    return int(data.mean())

def bench(algo, n, seed):
    binary = 'build/treeint'

    times = os.popen(f"taskset -c 15 ./{binary} {algo} {n} {seed}").read()
    times = np.fromstring(times, dtype=int, sep=',')
    times = times[:-1] # remove the last seperator
    l = int(len(times) / 3)
    times = times.reshape(l, 3)

    insert = times[:,0]
    find = times[:,1]
    remove = times[:,2]

    return [stat(insert), stat(find), stat(remove)]

# make sure we do make before everything start
os.system("make")

algo_list=["s-tree", "rbtree"]
nsize = list(k for k in range(50, 100000, 50))
ts = np.array([[bench(algo, size, size) for size in nsize] for algo in algo_list])

pat_name = ["insert", "find", "remove"]
fig, ax = plt.subplots(3, figsize=(6, 10))
for pat in range(0, 3):
    for idx, t in enumerate(ts):
        ax[pat].plot(nsize, t[:,pat], label=algo_list[idx])
    ax[pat].set_title(pat_name[pat])
    ax[pat].set_ylim(bottom=0, top=None)
    ax[pat].legend()

plt.ylabel('Latency(ns)')
plt.xlabel('Tree size')
plt.show()
