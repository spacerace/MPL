#  PROG.MAK
.SUFFIXES : .exe .obj .c .for .asm .y .l
VPATH     = .;\msc\include
HPATH     =

# Macros for compilers and assemblers.
AS        = masm
LEX       = lex
YACC      = yacc
DEBUG     = -Zi -Od
BIN       =
ECHO      = >stdout
CFLAGS    = -A$(MODEL) -J -DLINT_ARGS $(DEBUG) $(HPATH)
MODEL     = S
VARSTK    = $(LIB)$(MODEL)VARSTK
SETARGV   = $(LIB)$(MODEL)SETARGV
HOMEDIR   = .
DEST      = $(HOMEDIR)
EXTHDRS   =
HDRS      =
LDFLAGS   =
LIBS      =
LINKER    = link
LINKFLAGS = /CO
LINTFLAGS = -m$(MODEL) -EM -c
LINTLIST  = lintlist
MAKEFILE  = makefile
OBJS      = CXREF.OBJ \
		GOTOXY.OBJ \
		STRICMP.OBJ \
		WHEREY.OBJ
PRINT     = cpr -n
PROGRAM   = cxref.exe
SRCS      = CXREF.C \
		GOTOXY.C \
		STRICMP.C \
		WHEREY.C
VERSION   = msc
BIN       =
LFLAGS    = /CO
CC        = cl

all:  $(PROGRAM)

$(PROGRAM):     $(OBJS) $(LIBS)
		@echo "Loading $(PROGRAM) ... "
		@$(LINKER) $(OBJS) $(SETARGV),  $(PROGRAM), $(LIBS) $(LINKFLAGS);
		@echo "done"

co:;  @co -r$(VERSION) $(HDRS) $(SRCS)

depend:;	@mkmf -f $(MAKEFILE) PROGRAM=$(PROGRAM) \
		'DEST=$$(HOMEDIR)' HOMEDIR=`echo $$HOME`

diff:;   @vdiff -r$(VERSION) $(HDRS) $(SRCS)

index:;		@ctags -wx $(HDRS) $(SRCS)

install:	$(PROGRAM)
		@echo Installing $(PROGRAM) in $(DEST)
		@install $(IFLAGS) $(PROGRAM) $(DEST)

lint:;   @prec $(LINTFLAGS) $(SRCS) >$(LINTLIST)

print: $(HDRS) $(SRCS)
   @del list
   !@$(PRINT) $? >>list

vcs:  $(HDRS) $(SRCS)
   !put -b ($?)
   touch vcs

program: $(PROGRAM)

tags: $(HDRS) $(SRCS); @ctags $(HDRS) $(SRCS)

update:  $(DEST)/$(PROGRAM)

$(DEST)/$(PROGRAM):  $(SRCS) $(LIBS) $(HDRS) $(EXTHDRS)
		@make -f $(MAKEFILE) DEST=$(DEST) install tags

# DEFAULT RULES
#
#	ASM -> OBJ using MASM.
.asm.obj:
	${AS} $<;

#	C -> OBJ using Microsoft C. 
.c.obj:
	${CC} ${CFLAGS} -c $<

#	FOR -> OBJ using Microsoft Fortran.
.for.obj:
	for1 $<;
	pas2

# 	Y -> C, for YACC
.y.c:
	$(YACC) $(YFLAGS) $<
	rename ytab.c $@

#	L -> C, for LEX
.l.c:
	$(LEX) $(LFLAGS) $<
	mv lex.yy.c $@


# To produce a `.exe' file from an `.obj' file.  Note that there is a
# problem because LIBS may be different for linking `.obj' files
# produced by different compilers (C, FORTRAN, PASCAL, etc).  To avoid
# this problem you may want to have the C compiler produce `.cbj' files,
# the FORTRAN compiler produce `.fbj' files, etc.  Then you could write
# specific rules for `.cbj.exe' and `.fbj.exe' which would use the correct
# libraries.
.obj.exe:
	link $< $(SETARGV), $@,, $(LIBS) $(LINKFLAGS);

# To produce a `.exe' file from a `.asm' file.
.asm.exe:
	${AS} $<;
	link $*.obj, $@,, $(LIBS)
	erase $*.obj

# To produce a `.exe' file from a `.c' file.
.c.exe:
	${CC} $(CFLAGS) -c $<
	link $*.obj $(SETARGV), $@,, $(LIBS) $(LINKFLAGS);
	erase $*.obj

# To produce a `.exe' file from a `.for' file.
.for.exe:
	for1 $<;
	pas2
	link $*.obj, $@,, $(LIB)\FORTRAN
	erase $*.obj

# A universally useful target so "make clean" always works.
clean:
	-(erase *.bak; erase *.map)

large:
	(if exist \bin\c3s.exe do goto end; \
	 rename \bin\c3.exe c3s.exe; \
	 rename \bin\c3l.exe c3.exe; \
	 :end)
small:
	(if exist \bin\c3s.exe do goto end; \
	 rename \bin\c3.exe c3l.exe; \
	 rename \bin\c3s.exe c3.exe; \
	 :end)

chkdsk:
	chkdsk d:

###
