# makefile for the ViewFile (VF) program
# (compile using "compact" memory model)

MODEL = C
LIB = c:\lib
LLIB = c:\lib\local
LIBS = $(LLIB)\$(MODEL)util.lib $(LLIB)\$(MODEL)dos.lib $(LLIB)\$(MODEL)bios.lib
OBJS = vf_list.obj vf_dspy.obj vf_srch.obj vf_util.obj vf_cmd.obj \
	message.obj getstr.obj

vf_list.obj:	vf_list.c vf.h

vf_dspy.obj:	vf_dspy.c vf.h

vf_srch.obj:	vf_srch.c vf.h

vf_util.obj:	vf_util.c vf.h

vf_cmd.obj:	vf_cmd.c vf.h

getstr.obj:	getstr.c

message.obj:	message.c message.h

cvf.lib:	$(OBJS)
	del cvf.lib
	lib cvf +$(OBJS);

vf.obj:	vf.c

vf.exe:	vf.obj cvf.lib $(LIBS)
	link $* $(LIB)\csetargv, $*, nul, cvf $(LIBS);
