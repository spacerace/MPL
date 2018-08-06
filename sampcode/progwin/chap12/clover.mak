#----------------------
# CLOVER.MAK make file
#----------------------

clover.exe : clover.obj clover.def
     link clover, /align:16, NUL, /nod slibcew win87em libw, clover
     rc clover.exe

clover.obj : clover.c
     cl -c -Gsw -Ow -W2 -Zp clover.c
