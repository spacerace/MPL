#-----------------------
# POPPAD1.MAK make file
#-----------------------

poppad1.exe : poppad1.obj poppad1.def
     link poppad1, /align:16, NUL, /nod slibcew libw, poppad1.def
     rc poppad1.exe

poppad1.obj : poppad1.c
     cl -c -Gsw -Ow -W2 -Zp poppad1.c
