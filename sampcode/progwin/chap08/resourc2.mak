#------------------------
# RESOURC2.MAK make file
#------------------------

resourc2.exe : resourc2.obj resourc2.def resourc2.res
     link resourc2, /align:16, NUL, /nod slibcew libw, resourc2
     rc resourc2.res

resourc2.obj : resourc2.c
     cl -c -Gsw -Ow -W2 -Zp resourc2.c

resourc2.res : resourc2.rc resourc2.ico resourc2.cur resourc2.bmp
     rc -r resourc2.rc
