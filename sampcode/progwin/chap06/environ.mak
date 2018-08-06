#-----------------------
# ENVIRON.MAK make file
#-----------------------

environ.exe : environ.obj environ.def
     link environ, /align:16, NUL, /nod slibcew libw, environ
     rc environ.exe

environ.obj : environ.c
     cl -c -Gsw -Ow -W2 -Zp environ.c
