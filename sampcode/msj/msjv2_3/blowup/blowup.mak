# MAKE File for BLOWUP


blowup.obj : blowup.c
     cl -c -d -D LINT_ARGS -Gsw -Os -W2 -Zdp blowup.c

blowup.exe : blowup.obj blowup.def
     link4 blowup, /align:16, /map /line, slibw, blowup
     mapsym blowup
