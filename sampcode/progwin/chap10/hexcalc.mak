#-----------------------
# HEXCALC.MAK make file
#-----------------------

hexcalc.exe: hexcalc.obj hexcalc.def hexcalc.res
     link hexcalc, /align:16, NUL, /nod slibcew libw, hexcalc
     rc hexcalc.res

hexcalc.obj: hexcalc.c
     cl -c -Gsw -Ow -W2 -Zp hexcalc.c

hexcalc.res : hexcalc.rc hexcalc.ico
     rc -r hexcalc.rc
