#!/usr/bin/env python
import struct
from matplotlib import pyplot as plt
import numpy as np
import sys

# data = []
# with open("example.res", mode='rb') as f:
#     d=f.read(8)
#     c=0
#     while d:
#         try:
#             data[c].append(struct.unpack('>d',d))
#         except:
#             pass
#         d=f.read(8)
# print("decode :", data)

x = np.fromfile("example.res", dtype=float)
print(x, type(x))