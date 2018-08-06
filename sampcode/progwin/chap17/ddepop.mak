#----------------------
# DDEPOP.MAK make file
#----------------------

ddepop.exe : ddepop.obj ddepop.def ddepop.res
     link ddepop, /align:16, NUL, /nod slibcew win87em libw, ddepop
     rc ddepop.res

ddepop.obj : ddepop.c
     cl -c -Gsw -Ow -W2 -Zp ddepop.c

ddepop.res : ddepop.rc ddepop.ico
     rc -r ddepop.rc
