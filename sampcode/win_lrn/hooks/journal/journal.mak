
cp=cl -d -c -Asnw -Gw -Os -Zpe -W2

.c.obj:
    $(cp) $*.c

JOURNAL.obj: JOURNAL.c

libinit.obj : libinit.asm
    masm -i\include libinit.asm;

JOURNAL.exe: JOURNAL.obj libinit.obj JOURNAL.def
    link4 @JOURNAL.rsp
    mapsym JOURNAL
