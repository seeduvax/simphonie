#!/usr/bin/env python
import struct
from matplotlib import pyplot as plt
import numpy as np
import sys
import csv

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
        self.ftype = None
        self.data = []
        self.codeUnpack = None
        self.dataSize = None

    def setName(self, name):
        self.name = name

    def setDType(self, ftype):
        ftype=ftype.upper()
        self.ftype = ftype
        self.codeUnpack = SmpTypeToPythonUnPackCodeMap[ftype]
        self.dataSize = SmpTypeToDataSizeMap[ftype]

    def addValue(self, value):
        self.data.append(value)

    def addBinaryValue(self, dbytes):
        self.addValue(struct.unpack(self.codeUnpack,dbytes)[0])

    def decode(self, fd):
        data = fd.read(self.dataSize)
        self.addBinaryValue(data)
    
    def __str__(self):
        dataStr=";".join(map(lambda x:str(x), self.data))
        return "[Field] name:%s,type:%s,data:%s"%(self.name,self.ftype,dataStr)

class File:
    def __init__(self, name):
        self.name = name
        self.fields = []
    
    def getField(self, fieldName):
        mFields = isinstance(fieldName, list)
        for field in self.fields:
            if not mFields and fieldName == field.name:
                return field
            elif mFields and field.name in fieldName:
                return field
    
    def getFieldsName(self):
        fieldsName = []
        for f in self.fields:
            fieldsName.append(f.name)
        return fieldsName
    
    def __str__(self):
        field_list = self.getFieldsName()
        res = self.name + ": ["
        for f in field_list:
            res += f + ";"
        res = res[:-1] + "]"
        return res

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

def getDataFromBin(binaryFilePath):
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

    return fields
    fd.close()

def getDataFromCSV(CSVFilePath):
    fields = []
    with open(CSVFilePath) as csv_file:
        csv_reader = csv.reader(csv_file, delimiter=';')
        fields = []
        for row_idx, row in enumerate(csv_reader):
            # Add Fields
            if row_idx == 0:
                for column in row:
                    field = Field()
                    #first column begin by # (ignore it)
                    if len(fields) == 0 :
                        field.setName(column[1:])
                    else:
                        field.setName(column)
                    fields.append(field)
            # Add Data
            elif row_idx > 1 :
                for col_idx, column in enumerate(row):
                    val =  float(column)
                    if col_idx == 0:
                        val /= 1e9
                    data["data"][fields[col_count]].append(val)
                    fields[col_idx].data.append(val)
    return fields

def plotFields(files, displayFields, title, nbGraphs, graphIdx):
    global plt
    plt.subplot(nbGraphs,1,graphIdx)
    for file in files:
        plt.plot(file.getField("SimulationTime"), file.getField(displayFields), label=file.name)
    plt.title(title)

def compareData(files, precision=0):
    # global data
    data1 = files[0].data
    for file_idx in range(1,len(files)):
        for data in files[file].data:
            for val_idx, val in enumerate(data):
                if abs(val - data1[val_idx]) > precision:
                    print("Value line", lineval_idx + 1, "is not equal")
                    return False

# CSV file
files = []
for arg in sys.argv:
    if str(arg[-3:]) == ".py":
        pass
    elif len(str(arg)) > 4 and str(arg[-4:]) in [".csv",".CSV"]:
        f = File(str(arg))
        f.fields = getDataFromCSV(str(arg))
        files.append(f)
    elif len(str(arg)) > 4 and str(arg[-4:]) == ".res":
        f = File(str(arg))
        f.fields = getDataFromBin(str(arg))
        files.append(f)
    else:
        print("Can't read " + str(arg[-4:]) + ", file format not supported")
        exit()

for f in files:
    print("Fields of each files :")
    print(" •",f)

#compareData(0)
plotFields(files, ["dyn_intg.a[2]", "dyn_intg.a.a[2]"], "Acceleration", 5, 1)
plotFields(files, ["dyn_intg.p[2]", "dyn_intg.p.p[2]"], "SimPosition", 5, 2)
plotFields(files, ["ocs.p[2]", "av_tm.pos[2]", "ocs.p.p[2]"], "FswPosition", 5, 3)
plotFields(files, ["tank.flow"], "Flow", 5, 4)
plotFields(files, ["tank.totalMass"], "Mass", 5, 5)
plt.tight_layout()
plt.legend()
plt.show()
