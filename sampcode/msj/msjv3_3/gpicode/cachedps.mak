# Figure 3
# =========

#--------------------
# CACHEDPS make file
#--------------------

cachedps.obj : cachedps.c
     cl -c -G2sw -W2 -Zp cachedps.c

cachedps.exe : cachedps.obj cachedps.def
     link cachedps, /align:16, NUL, os2, cachedps
