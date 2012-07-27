#!/usr/bin/env python

import sys
import struct

import serial
import Image

def main():
    speed = 100000
    im = Image.new("RGB", (400,300))
    pix = im.load()
    print "listening for screenshot on", sys.argv[1]
    ser = serial.Serial(sys.argv[1], speed)

    while True:
        s = ser.read(1)

        chk = ord(s)
        if chk != 0xa5:
            continue
            #print "did not recieve correct header"
            #print str(hex(chk))
        else:
            break

    print "found header"
    (frame,) = struct.unpack("<H", ser.read(2))
    filename = "screenshot%05d.png" % frame
    print "saving: %s" % filename
    for yi in range(300):
        print yi
        (y,) = struct.unpack("H", ser.read(2))
        x = 0
        while x < 400:
            (v,) = struct.unpack("H", ser.read(2))
            if v & 0x8000:
                for i in range(v & 0x7fff):
                    pix[x, y] = 0;
                    x += 1
            else:
                r = (v >> 10) & 31
                g = (v >> 5) & 31
                b = v & 31
                pix[x, y] = (r * 8, g * 8, b * 8)
                x += 1
    im.save(filename)
    print "captured screenshot to", filename

if __name__ == "__main__":
    main()
