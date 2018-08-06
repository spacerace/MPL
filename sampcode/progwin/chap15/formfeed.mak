#------------------------
# FORMFEED.MAK make file
#------------------------

formfeed.exe: formfeed.obj formfeed.def
     link formfeed, /align:16, NUL, /nod slibcew libw, formfeed
     rc formfeed.exe

formfeed.obj: formfeed.c
     cl -c -Gsw -Ow -W2 -Zp formfeed.c
