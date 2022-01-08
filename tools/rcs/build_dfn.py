#!/usr/bin/env python3
import sys
from struct import *

hfilename = sys.argv[1]
dfnfilename = hfilename+".dfn"

hfile = open(hfilename, "r")
kind_dict = {
    "TREE*/":3,             # Dialog (or menu or panel)
    "STRING*/":4,           # Alert
    "OBJECTin":0x100,       # Object in tree  
    "FREESTRING":0x101,     # Free String
    "FREEIMAGE":0x102,      # Free Image
}

defs=[]

linenum=0
for line in hfile:
    linenum += 1
    token = line.split()
    # print(token)
    try:
        kind = kind_dict[token[4]+token[5]]
        value = int(token[2])
        # for OBJECTs, set the high byte to the parent tree
        if kind == 0x100:
            print("OBJECT in tree", token[7], "line", linenum, "old value:", value)
            value += (int(token[7][1:]) << 8)
            print("new value:", value)
        bytepack = pack("<HH10s", value,kind,token[1].encode('ascii'))
        # print(bytepack)
        defs.append(bytepack)
    except KeyError:
        print("UNKNONW TYPE in line", linenum)

hfile.close()

dfnfile = open(dfnfilename, "wb")
dfnsize = len(defs)
print("found", dfnsize, "definitions in header file")
print("writing dfn file")
dfnfile.write(pack("<H",dfnsize))
for bytepack in defs:
    dfnfile.write(bytepack)

dfnfile.close()
