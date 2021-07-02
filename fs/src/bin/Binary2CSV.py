#!/usr/bin/env python
import struct
from matplotlib import pyplot as plt
import numpy as np
import sys

binaryFilePath = sys.argv[1]

SmpTypeToPythonUnPackCodeMap={
    "PTK_INT64":'q',
    "PTK_UINT64":'Q',
    "PTK_INT32":'i',
    "PTK_UINT32":'I',
    "PTK_FLOAT32": 'f',
    "PTK_FLOAT64": 'd'
}
SmpTypeToDataSizeMap={
    "PTK_INT64":8,
    "PTK_UINT64":8,
    "PTK_INT32":4,
    "PTK_UINT32":4,
    "PTK_FLOAT32": 4,
    "PTK_FLOAT64": 8
}

class Field:
    def __init__(self):
        self.name = ""
        self.dtype = None
        self.data = []
        self.codeUnpack = None
        self.dataSize = None

    def setName(self, name):
        self.name = name

    def setDType(self, dtype):
        dtype=dtype.upper()
        self.dtype = dtype
        self.codeUnpack = SmpTypeToPythonUnPackCodeMap[dtype]
        self.dataSize = SmpTypeToDataSizeMap[dtype]

    def addValue(self, value):
        self.data.append(value)
        print("data :", value)

    def addBinaryValue(self, dbytes):
        self.addValue(struct.unpack(self.codeUnpack,dbytes)[0])

    def decode(self, fd):
        data = fd.read(self.dataSize)
        self.addBinaryValue(data)
    
    def __str__(self):
        dataStr=";".join(map(lambda x:str(x), self.data))
        return "[Field] name:%s,type:%s,data:%s"%(self.name,self.dtype,dataStr)


def getString(fd):
    word=""
    while True:
        byte=fd.read(1)
        if byte==b'\x00':
            break
        else:
            word+=byte.decode()
    return word


def discoverFieldNames(fd, fields):
    idx = 0
    while True:
        byte=fd.peek(1)[:1]
        if byte==b'\x01':
            fd.read(1) # to avance cursor
            break
        else:
            fieldName = getString(fd)
            print("field %d : %s" % (idx, fieldName))
            field = Field()
            field.setName(fieldName)
            fields.append(field)
            idx+=1


def discoverFieldTypes(fd, fields):
    idx = 0
    while True:
        byte=fd.peek(1)[:1]
        if byte==b'\x01':
            fd.read(1) # to avance cursor
            break
        else:
            fieldType = getString(fd)
            print("field type %d : %s" % (idx, fieldType))
            fields[idx].setDType(fieldType)
            idx+=1


def discoverFieldData(fd, fields):
    for idx,field in enumerate(fields):
        field.decode(fd)
        byteSep = fd.read(1)
        if byteSep == b'\x00':
            continue
        elif byteSep == b'\x01':
            assert idx==(len(fields)-1) , "Truncated buffer, some fields  weren't decoded"
        else:
            assert False, "Unexpected byte separator"

def store(fields, FileName):
    csvfile = open(FileName, 'w')
    csvfile.write("#")

    line = ""
    # write fields name
    for f in fields:
        line += f.name + ";"
    csvfile.write(line[:-1] + "\n")
    
    line = ""
    # write fields types
    for f in fields:
        line += f.dtype + ";"
    csvfile.write(line[:-1] + "\n")

    for col in range(0,len(fields[0].data)):
        line = ""
        for f in fields:
            line += str(f.data[col]) + ";"
        csvfile.write(line[:-1] + "\n")
    csvfile.close()


def main():
    print("Opening binary file %s" % binaryFilePath)
    fields=[]
    fd=open(binaryFilePath,"rb")
    byte=fd.read(1)
    assert byte==b'\x01', "Expecting 'x01' as beginning separator"

    print("discovering field names")
    discoverFieldNames(fd, fields)

    print("discovering field types")
    discoverFieldTypes(fd, fields)

    print("discovering field data")
    while True:
        byte=fd.peek(1)[:1]
        if len(byte)==0:
            break
        else:
            discoverFieldData(fd, fields)

    store(fields, binaryFilePath[:-3] + "csv")
    fd.close()
main()


