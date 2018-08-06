# Figure 3 
# ========

#-------------------
# BIGJOB2 make file
#-------------------

bigjob2.obj : bigjob2.c bigjob.h
     cl -c -FPa -G2sw -W3 -Zp bigjob2.c

bigjob.res : bigjob.rc bigjob.h
     rc -r bigjob.rc

bigjob2.exe : bigjob2.obj bigjob2.def bigjob.res
     link bigjob2, /align:16, /map, /nod slibc slibcp slibfa os2, bigjob2
     rc bigjob.res bigjob2.exe
