
#
#  Standard command line definitions
#

cp=cl -d -c -AS -Gsw -Os -Zpei

#
#  Standard inference rules
#
.c.obj:
    $(cp) $*.c

#
#  The C File List
#

TTY.obj: TTY.c TTY.h

TTY.res: TTY.rc TTY.ico TTY.h
    rc -r TTY.rc

TTY.exe: TTY.obj TTY.res TTY.def
    link4 /NOE/CO TTY,/align:16,/map,slibw,TTY.def
    rc TTY.res
