#------------------------
# HELLOWIN.MAK make file
#------------------------

hellowin.exe : hellowin.obj hellowin.def
     link hellowin, /align:16, NUL, /nod slibcew libw, hellowin
     rc hellowin.exe

hellowin.obj : hellowin.c
     cl -c -Gsw -Ow -W2 -Zp hellowin.c
