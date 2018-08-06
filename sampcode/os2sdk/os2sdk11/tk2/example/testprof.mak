#   ------------  testprof.exe makefile   Test Profile Sample Program
#   ------------

PROINC = ../
CFLAGS = -c -G2s -Zep -Od -DLINT_ARGS -DDEBUG -DPROFILE -I$(PROINC)
AFLAGS = -Mx -t


# ***** Rules

.c.obj:
	cl $(CFLAGS) $*.c

.asm.obj:
	masm $(AFLAGS) $*.asm;


# ***** Dependencies

testprof.obj:	testprof.c $(PROINC)profile.h


# ***** LINK step

testprof.exe: testprof.obj testprof.lrf testprof.def
	link @testprof.lrf




