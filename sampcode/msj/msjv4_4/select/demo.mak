demo.res: demo.dlg demo.rc demo.h
  rc -r demo.rc

demo.obj: demo.c demo.h
  cl -Zi -c -AS -Gsw -G2 -Ownlt -Zpe -W2 demo.c

demo.exe: demo.obj demo.res demo.def
  link4 demo/co, demo.exe/NOD, demo.map/map, slibw slibcew, demo.def
  rc demo.res

