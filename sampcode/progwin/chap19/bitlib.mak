#----------------------
# BITLIB.MAK make file
#----------------------

bitlib.dll : bitlib.obj bitlib.def bitlib.res
     link bitlib libentry, bitlib.dll /align:16, NUL, /nod sdllcew libw, bitlib
     rc bitlib.res bitlib.dll

bitlib.obj : bitlib.c
     cl -c -ASw -Gsw -Ow -W2 -Zp bitlib.c

bitlib.res : bitlib.rc
     rc -r bitlib.rc
