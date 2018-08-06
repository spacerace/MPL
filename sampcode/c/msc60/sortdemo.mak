PROJ = SORTDEMO
PROJFILE = SORTDEMO.MAK
DEBUG = 1

PWBRMAKE  = pwbrmake
NMAKE  = nmake
LINKER  = link
ILINK  = ilink
LRF  = echo > NUL
BIND  = bind
RC  = rc
IMPLIB  = implib
LFLAGS_G  =  /NOI /NOE  /BATCH
LFLAGS_D  = /CO /INC /FAR /PACKC
LFLAGS_R  = /EXE /FAR /PACKC
MAPFILE_D  = NUL
MAPFILE_R  = NUL
CC  = cl
CFLAGS_G  = /W2 /BATCH
CFLAGS_D  = /qc /Gi$(PROJ).mdt /Zr /Zi /Od
CFLAGS_R  = /Ot /Oi /Ol /Oe /Og /Gs
LLIBS_R  =  /NOD:SLIBCE SLIBCER
LLIBS_D  =  /NOD:SLIBCE SLIBCER
ASM  = masm
AFLAGS_G  = /Mx /T
AFLAGS_D  = /Zi
LLIBS_G  = graphics.lib

OBJS  = SORTDEMO.obj
SBRS  = SORTDEMO.sbr

all: $(PROJ).exe

.SUFFIXES: .c .sbr .obj

SORTDEMO.obj : SORTDEMO.C

SORTDEMO.sbr : SORTDEMO.C


$(PROJ).bsc : $(SBRS)
        $(PWBRMAKE) @<<
$(BRFLAGS) $(SBRS)
<<

$(PROJ).exe : $(OBJS)
!IF $(DEBUG)
        $(LRF) @<<$(PROJ).lrf
$(RT_OBJS) $(OBJS: = +^
),$@,$(MAPFILE_D),$(LLIBS_G) $(LLIBS_D) $(LIBS),$(DEF_FILE) $(LFLAGS_G) $(LFLAGS_D);
<<
!ELSE
        $(LRF) @<<$(PROJ).lrf
$(RT_OBJS) $(OBJS: = +^
),$@,$(MAPFILE_R),$(LLIBS_G) $(LLIBS_R) $(LIBS),$(DEF_FILE) $(LFLAGS_G) $(LFLAGS_R);
<<
!ENDIF
!IF $(DEBUG)
        $(ILINK) -a -e "$(LINKER) @$(PROJ).lrf" $@
!ELSE
        $(LINKER) @$(PROJ).lrf
!ENDIF


.c.sbr :
!IF $(DEBUG)
        $(CC) /Zs $(CFLAGS_G) $(CFLAGS_D) /FR$@ $<
!ELSE
        $(CC) /Zs $(CFLAGS_G) $(CFLAGS_R) /FR$@ $<
!ENDIF

.c.obj :
!IF $(DEBUG)
        $(CC) /c $(CFLAGS_G) $(CFLAGS_D) /Fo$@ $<
!ELSE
        $(CC) /c $(CFLAGS_G) $(CFLAGS_R) /Fo$@ $<
!ENDIF


run: $(PROJ).exe
        $(PROJ).exe $(RUNFLAGS)

debug: $(PROJ).exe
        CV $(CVFLAGS) $(PROJ).exe $(RUNFLAGS)
