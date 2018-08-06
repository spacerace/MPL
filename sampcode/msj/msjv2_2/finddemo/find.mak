# ============================================================================
# ============================================================================
# Figure 7: Make-file for FINDDEMO.EXE and FINDER.EXE

# ============================================================================

finddemo.obj  :  finddemo.
    cl -c -G2 -Zp finddemo.c

finddemo.exe  :  finddemo.obj finddemo.def
    link finddemo, /align:16, /map, doscalls slibc5, finddemo.def

finder.obj  :  findef.c
    cl -c -G2 -Zp finder.c

finder.exe  :  finder.obj finder.def
    link finder, /align:16, /map, doscalls slibc5, finder.def
