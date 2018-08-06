#-----------------------
# CONNECT.MAK make file
#-----------------------

connect.exe : connect.obj connect.def
     link connect, /align:16, NUL, /nod slibcew libw, connect
     rc connect.exe

connect.obj : connect.c
     cl -c -Gsw -Ow -W2 -Zp connect.c
