# Figure 2w
# =========

#--------------------------------------
# HEXCALCW make file (Windows version)
#--------------------------------------

hexcalcw.obj: hexcalcw.c
     cl -c -D LINT_ARGS -Gsw -Os -W2 -Zp hexcalcw.c

hexcalcw.res : hexcalcw.rc hexcalcw.ico
     rc -r hexcalcw.rc

hexcalcw.exe: hexcalcw.obj hexcalcw.def hexcalcw.res
     link4 hexcalcw, /align:16, /map, slibw, hexcalcw
     rc hexcalcw.res
