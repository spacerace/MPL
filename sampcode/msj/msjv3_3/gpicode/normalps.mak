# Figure 6
# ========

#--------------------
# NORMALPS make file
#--------------------

normalps.obj : normalps.c
     cl -c -G2sw -W2 -Zp normalps.c

normalps.exe : normalps.obj normalps.def
     link normalps, /align:16, NUL, os2, normalps

