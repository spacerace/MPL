# Figure 4
# ========

#-------------------
# MICROPS make file
#-------------------

microps.obj : microps.c
     cl -c -G2sw -W2 -Zp microps.c

microps.exe : microps.obj microps.def
     link microps, /align:16, NUL, os2, microps
