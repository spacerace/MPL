CC  = cl -AS   -c -u -Gsw -Od -Zpe
CCL = cl -Alnw -c -u -Gsw -Od -Zpe
DFLAGS = -DTEST

all: ddespy.exe

libinit.obj : libinit.asm
    masm -i\include libinit.asm;

ddespy.res: ddespy.rc ddespy.ico ddespy.h
        rc -r ddespy.rc

ddespy.obj: ddespy.c ddespy.h
        $(CC) $(DFLAGS) ddespy.c

ddespy.exe: ddespy.obj ddespy.res ddespy.def
        link4 ddespy, ddespy/align:16, ddespy/map, slibw/NOE, ddespy.def
        rc ddespy.res

dspydll.obj: dspydll.c ddespy.h
        $(CCL) dspydll.c

dspydll.exe: dspydll.obj libinit.obj dspydll.def
    link4 libinit dspydll, dspydll.exe/align:16,,lwinlibc llibw/NOE, dspydll.def



