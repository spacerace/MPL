CFLAGS	= -W2 -Zpi

send.obj: send.asm
	masm /MX send.asm;

jrnl.exe: send.obj
	cl $(CFLAGS) jrnl.c send.obj
