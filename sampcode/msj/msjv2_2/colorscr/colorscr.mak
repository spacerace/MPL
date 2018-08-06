# ============================================================================
# ============================================================================
# Figure 8: Make-file for COLORSCR.EXE

# ============================================================================

colorscr.obj  :  colorscr.c
    cl -c -d -Gsw -Os -W2 -Zpd colorscr.c

colorscr.exe  :  colorscr.obj colorscr.def
    link4 colorscr, /align:16, /map /line, slibw, colorscr
    mapsym colorscr
