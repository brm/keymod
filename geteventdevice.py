#!/usr/bin/env python

import sys
import pprint

def getInfoBlocks(lines):
    blocks = []
    curInfo = { }
    for l in lines:
        l = l.strip()
        if len(l) == 0:
            if curInfo:
                blocks.append(curInfo)
            curInfo = { }
        else:
            if l.startswith("I: "):
                curInfo["info"] = l[3:]
            elif l.startswith("N: Name="):
                name = l[8:]
                if name[0] == "\"" and name[-1] == "\"":
                    name = name[1:-1]
                curInfo["name"] = name
            elif l.startswith("H: Handlers="):
                hStr = l[12:]
                curInfo["handlers"] = hStr.split()

    if curInfo:
        blocks.append(curInfo)

    return blocks

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print >>sys.stderr, "Usage: %s \"Identifying string from /proc/bus/input/devices\"" % sys.argv[0]
        sys.exit(-1)

    s = sys.argv[1]
    with open("/proc/bus/input/devices", "rt") as f:
        lines = f.readlines()

    blocks = getInfoBlocks(lines)
    eventName = "not-found"
    for b in blocks:
        if s in b["name"]:
            for h in b["handlers"]:
                if h.startswith("event"):
                    eventName = h
                    break

    print "/dev/input/%s" % eventName
