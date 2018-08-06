#-----------------------
# FREEMEM.MAK make file
#-----------------------

freemem.exe : freemem.obj freemem.def
     link freemem, /align:16, NUL, /nod slibcew win87em libw, freemem
     rc freemem.exe

freemem.obj : freemem.c
     cl -c -Gsw -Ow -W2 -Zp freemem.c
