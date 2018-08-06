#------------------------
# ROP2LOOK.MAK make file
#------------------------

rop2look.exe : rop2look.obj rop2look.def rop2look.res
     link rop2look, /align:16, NUL, /nod slibcew libw, rop2look
     rc rop2look.res

rop2look.obj : rop2look.c
     cl -c -Gsw -Ow -W2 -Zp rop2look.c

rop2look.res : rop2look.rc
     rc -r rop2look.rc
