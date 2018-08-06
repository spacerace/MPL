#------------------------
# RESOURC1.MAK make file
#------------------------

resourc1.exe : resourc1.obj resourc1.def resourc1.res
     link resourc1, /align:16, NUL, /nod slibcew libw, resourc1
     rc resourc1.res

resourc1.obj : resourc1.c
     cl -c -Gsw -Ow -W2 -Zp resourc1.c

resourc1.res : resourc1.rc resourc1.ico resourc1.cur
     rc -r resourc1.rc
