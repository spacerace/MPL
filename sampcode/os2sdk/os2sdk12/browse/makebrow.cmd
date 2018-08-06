cd avbrowse
nmake browse
del *.obj *.res *.map
cd ..\vbrowse
nmake browse
del *.obj *.map
cd ..
