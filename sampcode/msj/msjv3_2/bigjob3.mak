# Figure 4 
# ========

#-------------------
# BIGJOB3 make file
#-------------------

bigjob3.obj : bigjob3.c bigjob.h
     cl -c -FPa -G2sw -W3 -Zp bigjob3.c

bigjob.res : bigjob.rc bigjob.h
     rc -r bigjob.rc

bigjob3.exe : bigjob3.obj bigjob3.def bigjob.res
     link bigjob3, /align:16, /map, /nod slibc slibcp slibfa os2, bigjob3
     rc bigjob.res bigjob3.exe
