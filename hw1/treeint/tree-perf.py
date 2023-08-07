#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt
import os

def stat(data):
    data = np.array(data)
    mean, std = data.mean(), data.std()
    # identify outliers: 2 std = 95%
    cut_off = std * 2
    lower, upper = mean - cut_off, mean + cut_off
    data = data[data > lower]
    data = data[data < upper]

    return int(data.mean())

def bench(algo, n):
    binary = 'build/treeint'

    # FIXME: I know it may not be a great idea to collect
    # data like this......
    insert = []
    find = []
    remove = []
    for _ in range(0, 100):
        times = os.popen(f"taskset -c 15 ./{binary} {algo} {n}").read()
        # strip out the '\n'
        times = times.strip()
        times = [int(x) for x in times.split(',')]

        insert.append(times[0])
        find.append(times[1])
        remove.append(times[2])

    return [stat(insert), stat(find), stat(remove)]

# make sure we do make before everything start
os.system("make")

algo_list=["s-tree", "rbtree"]
nsize = list(int(2**k) for k in range(5, 20))
ts = np.array([[bench(algo, size) for size in nsize] for algo in algo_list])

pat_name = ["insert", "find", "remove"]
fig, ax = plt.subplots(3)
for pat in range(0, 3):
    for idx, t in enumerate(ts):
        ax[pat].plot(nsize, t[:,pat], label=algo_list[idx])
    ax[pat].set_title(pat_name[pat])
    ax[pat].set_xscale('log', base=2)
    ax[pat].set_ylim(bottom=0, top=None)
    ax[pat].legend()

plt.ylabel('Latency(ns)')
plt.xlabel('Tree size')
plt.show()
