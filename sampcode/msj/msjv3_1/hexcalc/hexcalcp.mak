# Figure 2pm
# =========


#------------------------------------------------
# HEXCALCP make file (OS/2 Presentation Manager)
#------------------------------------------------

hexcalcp.obj: hexcalcp.c hexcalcp.h
     cl -c -D LINT_ARGS -G2sw -W2 -Zp hexcalcp.c

hexcalcp.res : hexcalcp.rc hexcalcp.h
     rc -r hexcalcp.rc

hexcalcp.exe: hexcalcp.obj hexcalcp.def hexcalcp.res
     link hexcalcp, /align:16, /map, slibc5 os2, hexcalcp
     rc hexcalcp.res
