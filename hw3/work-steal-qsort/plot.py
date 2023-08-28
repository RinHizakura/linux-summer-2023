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

def time(thread):
    binary = 'qsort-mt'
    cmd = f"build/{binary} -h {thread} -t"
    t = os.popen(cmd).read()
    # FIXME: We have some bug for the current implementation, which
    # only happen for enough trying. Just ignore it now but we need
    # to carefully handle it in the future
    while t == '':
        t = os.popen(cmd).read()
    return int(t)

os.system("make")

x = np.arange(1, 32)
y = []
for thread in x:
    collect = np.zeros(100)
    for idx in range(0, 100):
        collect[idx] = time(thread)
    y.append(stat(collect))

plt.plot(x, y)
plt.ylabel('Latency(ns)')
plt.xlabel('Thread number')
plt.xticks(x)
plt.show()
