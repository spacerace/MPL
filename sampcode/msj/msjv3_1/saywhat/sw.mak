# Figure 1w
# =========


#  MAKE file for SAYWHAT (Windows version)

sw.obj:  sw.c  sw.h
    cl -c -AS -DLINT_ARGS -Gcsw -Oas -W3 -Zdp sw.c

sw.res:  sw.rc  sw.h
    rc -r sw.rc

saywhat.exe:  sw.obj  sw.res  sw.def
    link4 sw, saywhat/align:16, saywhat/map/line, slibw, sw.def
    rc sw.res saywhat.exe
    mapsym saywhat
