# ============================================================================
# ============================================================================

# Figure 11: Make-File for WSZ
# ============================================================================

wsz.res: wsz.rc wsz.ico wsz.h
    rc -r wsz.rc

wsz.obj: wsz.c wsz.h
    cl -d -c -W2 -AS -Gs -Gw -Oas -Zpd -FPa wsz.c

wsz.exe: wsz.obj wsz.res wsz.def
    link4 wsz, wsz/align:16, wsz/map, slibw, wsz.def
    mapsym wsz
    rc wsz.res
