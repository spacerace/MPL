# Figure 5 
# ========


#-------------------
# BIGJOB4 make file
#-------------------

bigjob4.obj : bigjob4.c bigjob.h
     cl -c -FPa -G2sw -W3 -Zp bigjob4.c

bigjob.res : bigjob.rc bigjob.h
     rc -r bigjob.rc

bigjob4.exe : bigjob4.obj bigjob4.def bigjob.res
     link bigjob4, /align:16, /map, /nod slibc slibcp slibfa os2, bigjob4
     rc bigjob.res bigjob4.exe
