#------------------------
# DIGCLOCK.MAK make file
#------------------------

digclock.exe : digclock.obj digclock.def
     link digclock, /align:16, NUL, /nod slibcew libw, digclock
     rc digclock.exe

digclock.obj : digclock.c
     cl -c -Gsw -Ow -W2 -Zp digclock.c
