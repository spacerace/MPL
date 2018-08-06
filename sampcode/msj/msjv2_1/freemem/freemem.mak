# ============================================================================
# ============================================================================
# Figure 5: FREEMEM Make-File

# ============================================================================

# Make file for FREEMEM -- assumes Microsoft C 4.0
# ------------------------------------------------

freemem.obj : freemem.c
    cl -c -d -Gsw -Os -W2 -Zdp freemem.c

freemem.exe : freemem.obj freemem.def
    link4 freemem, /align:16, /map/line, slibw, freemem
    mapsym freemem
