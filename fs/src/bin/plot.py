#!/usr/bin/env python
import struct
from matplotlib import pyplot as plt
import numpy as np
import sys

fields=[]
data=[]
f=open(sys.argv[1],"rb")
try:
        byte=f.read(1)
        name=""
        mode=0
        unit=0
        while byte and mode<2:
            if mode==0:
                if byte=='\x01': # first time
                    mode=1
            elif mode==1:
                if byte=='\x01':
                    mode=2 # end of fields line
                elif byte=='\x00':
                    if unit==0:
                        fields.append(name.decode())
                        data.append([])
                        unit=1
                    else:
                        unit=0
                    name=""
                else:
                    name=name+byte
            if mode<2:
                byte=f.read(1)
        # -----------------------------------------------------
        d=f.read(8)
        c=0;
        nbc=len(fields)
        while d:
            try:
                data[c].append(struct.unpack('<d',d))
            except:
                pass
            c=c+1
            if (c>=nbc):
                c=0
            d=f.read(8)
            
finally:
    f.close()

for i in range(len(fields)):
	if fields[i]==u'dyn_intg.a[2]':
		accIndex=i
	elif fields[i]==u'dyn_intg.p[2]':
		simPosIndex=i
	elif fields[i]==u'ocs.p[2]' or fields[i]==u'navigate.locZ' or fields[i]==u'av_tm.pos[2]':
		fswPosIndex=i
	elif fields[i]==u'tank.flow':
		flowIndex=i
	elif fields[i]==u'tank.totalMass':
		massIndex=i

print(fields)
plt.subplot(4,1,1)
plt.plot(data[0],data[accIndex])
plt.subplot(4,1,2)
plt.plot(data[0],data[simPosIndex],label="sim")
plt.plot(data[0],data[fswPosIndex],label="pvol")
plt.subplot(4,1,3)
plt.plot(data[0],data[flowIndex])
plt.subplot(4,1,4)
plt.plot(data[0],data[massIndex])
plt.show()
