######################################################
#	Makefile for cpg
#		may require slight modifying for
#		some dos make's, will require a change
#		for unix make (.c.o inference)
#		for different compilers, just change the
#		CC, LD, LIB, and flags options.
#
######################################################

#	some system defaults
#
CC=\msc\msc
LD=\msc\cl
SRCLIST=nul.lst
OBJLIST=nul.cod
ASMLIST=
LIB=c:\msc\lib\small\slibc
# LINKFLAGS = -STACK:4000 -NOI -CPARMAXALLOC:1
LINKFLAGS = 
CFLAGS =

######################################################
#	Now, for some definitions 
######################################################
#	this will allow us to build arc files for distibution
#	
ARCPROD = cpg.arc
#
#	this isn't alwasy needed, but for arc's it's nice
DOC = cpg.doc
MAKEFILE = makefile

######################################################
#	this is a key entry, it's what we want when we're done
######################################################
PRODUCT=cpg.exe

######################################################
#	what we have to work with
#
SRC = cpg.c getopt.c

######################################################
#	what we want the intermediates to be
#
OBJS = cpg.obj getopt.obj

######################################################
#	how we convert the SRC to OBJ
#
*.obj : *.c
        $(CC) $(CFLAGS) $*.c,$*.obj,$(SRCLIST),$(OBJLIST);

######################################################
#	placed here for some compatibility with odd make's
all:	$(PRODUCT)

######################################################
#	finally, how we get from the OBJ's to the final executable
#
$(PRODUCT):		$(OBJS)
	$(LD)  $(LINKFLAGS) -Fo$(PRODUCT) $(OBJS) -link $(LIB);

	
######################################################
#	a little house cleaning...
#
clean:
	del  *.lst
	del  *.cod
	del  *.obj
	del  $(PRODUCT) err

######################################################
#	arc maint for distribution
#	NOTE:	because of the dependancy on $(MAKEFILE)
#		if it is touched, arc will force a complete
#		recompile to insure a current copy for
#		re-distribution!!  
#
arc: $(PRODUCT) $(SRC) $(MAKEFILE) $(DOC)
	del $(ARCPROD)
	arc a $(ARCPROD) $(PRODUCT) 
	arc a $(ARCPROD) $(SRC) 
	arc a $(ARCPROD) $(MAKEFILE) 
	arc a $(ARCPROD) $(DOC)
