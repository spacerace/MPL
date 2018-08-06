#----------------------
# POPPAD.MAK make file
#----------------------

poppad.exe : poppad.obj  poppadf.obj poppadp.obj \
             filedlg.obj poppad.def  poppad.res
     link poppad poppadf poppadp filedlg, poppad.exe /align:16, \
          NUL, /nod slibcew libw, poppad
     rc poppad.res  poppad.exe

poppad.obj : poppad.c poppad.h
     cl -c -Gsw -Ow -W2 -Zp poppad.c

poppadf.obj : poppadf.c
     cl -c -Gsw -Ow -W2 -Zp poppadf.c

poppadp.obj : poppadp.c
     cl -c -Gsw -Ow -W2 -Zp poppadp.c

filedlg.obj : filedlg.c filedlg.h
     cl -c -Gsw -Ow -W2 -Zp filedlg.c

poppad.res : poppad.rc poppad.h poppad.ico filedlg.dlg filedlg.h
     rc -r poppad.rc
