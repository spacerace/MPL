#-----------------------
# POEPOEM.MAK make file
#-----------------------

poepoem.exe : poepoem.obj poepoem.def poepoem.res
     link poepoem, /align:16, NUL, /nod slibcew libw, poepoem
     rc poepoem.res

poepoem.obj : poepoem.c poepoem.h
     cl -c -Gsw -Ow -W2 -Zp poepoem.c

poepoem.res : poepoem.rc poepoem.ico poepoem.asc poepoem.h
     rc -r poepoem.rc
