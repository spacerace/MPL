# Figure 6
# ========

#-------------------
# BIGJOB5 make file
#-------------------

bigjob5.obj : bigjob5.c bigjob.h
     cl -c -FPa -G2sw -W3 -Zp bigjob5.c

bigjob.res : bigjob.rc bigjob.h
     rc -r bigjob.rc

bigjob5.exe : bigjob5.obj bigjob5.def bigjob.res
     link bigjob5, /align:16, /map, /nod slibc slibcp slibfa os2, bigjob5
     rc bigjob.res bigjob5.exe
