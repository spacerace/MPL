PROJ	=CHRTDEMO
DEBUG	=0
CC	= qcl
CFLAGS_G	=
CFLAGS_D	=/Od /Zi /Zr /DDEBUG
CFLAGS_R	=/O /DNDEBUG
CFLAGS	=$(CFLAGS_G) $(CFLAGS_R)
LFLAGS_G	=/NOI
LFLAGS_D	=/CO
LFLAGS_R	=
LFLAGS	=$(LFLAGS_G) $(LFLAGS_R)
RUNFLAGS	=
OBJS_EXT = 	
LIBS_EXT = 	

all:	$(PROJ).exe

chrtdemo.obj:	chrtdemo.c

chrtsupt.obj:	chrtsupt.c

$(PROJ).exe:	chrtdemo.obj chrtsupt.obj $(OBJS_EXT)
	echo >NUL @<<$(PROJ).crf
chrtdemo.obj +
chrtsupt.obj +
$(OBJS_EXT)
$(PROJ).exe

$(LIBS_EXT);
<<
	link $(LFLAGS) @$(PROJ).crf

run: $(PROJ).exe
	$(PROJ) $(RUNFLAGS)

