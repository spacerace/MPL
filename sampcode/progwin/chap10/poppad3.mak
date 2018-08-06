#-----------------------
# POPPAD3.MAK make file
#-----------------------

poppad3.exe : poppad.obj  poppadf.obj poppadp0.obj \
              filedlg.obj poppad3.def poppad.res
     link poppad poppadf poppadp0 filedlg, poppad3.exe /align:16, \
          NUL, /nod slibcew libw, poppad3
     rc poppad.res  poppad3.exe

poppad.obj : poppad.c poppad.h
     cl -c -Gsw -Ow -W2 -Zp poppad.c

poppadf.obj : poppadf.c
     cl -c -Gsw -Ow -W2 -Zp poppadf.c

poppadp0.obj : poppadp0.c
     cl -c -Gsw -Ow -W2 -Zp poppadp0.c

filedlg.obj : filedlg.c filedlg.h
     cl -c -Gsw -Ow -W2 -Zp filedlg.c

poppad.res : poppad.rc poppad.h poppad.ico filedlg.dlg filedlg.h
     rc -r poppad.rc
