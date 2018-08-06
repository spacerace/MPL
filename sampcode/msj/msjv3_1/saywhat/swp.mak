# Figure 1pm
# =========


#  MAKE file for SAYWHAT (Presentation Manager version)

swp.obj:    swp.c  swp.h
    cl -c -AS -DLINT_ARGS -G2csw -Oat -W3 -Zp swp.c

swp.res:    swp.rc  swp.h
    rc -r swp.rc

saywhatp.exe:  swp.obj  swp.res  swp.def
    link @swp.lnk
    mapsym saywhat
    rc swp.res saywhat.exe
