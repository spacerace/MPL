# Figure 2 
# ========

#-------------------
# BIGJOB1 make file
#-------------------

bigjob1.obj : bigjob1.c bigjob.h
     cl -c -FPa -G2sw -W3 -Zp bigjob1.c

bigjob.res : bigjob.rc bigjob.h
     rc -r bigjob.rc

bigjob1.exe : bigjob1.obj bigjob1.def bigjob.res
     link bigjob1, /align:16, /map, /nod slibc slibcp slibfa os2, bigjob1
     rc bigjob.res bigjob1.exe
unCrunching: <to console> BIGJOB1.C
