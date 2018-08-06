#--------------------
# ARCS.MAK make file
#--------------------

arcs.exe : arcs.obj arcs.def arcs.res
     link arcs, /align:16, NUL, /nod slibcew libw, arcs
     rc arcs.res

arcs.obj : arcs.c arcs.h
     cl -c -Gsw -Ow -W2 -Zp arcs.c

arcs.res : arcs.rc arcs.h
     rc -r arcs.rc
