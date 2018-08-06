# makefile for the PR program

LINC=c:\include\local
LIB=c:\lib
LLIB=c:\lib\local

OBJS = pr_cpy.obj pr_file.obj pr_getln.obj pr_help.obj pr_gcnf.obj pr_line.obj

pr_cpy.obj:	pr_cpy.c print.h $(LINC)\printer.h

pr_file.obj:	pr_file.c print.h

pr_getln.obj:	pr_getln.c

pr_help.obj:	pr_help.c

pr_gcnf.obj:	pr_gcnf.c print.h $(LINC)\printer.h

pr_line.obj:	pr_line.c print.h

prlib.lib:	$(OBJS)
	del $*.lib
	lib $* +$(OBJS);

pr.obj:		pr.c print.h

pr.exe:		pr.obj prlib.lib $(LLIB)\sutil.lib
	link $* $(LIB)\ssetargv, $*, nul, prlib $(LLIB)\sutil;
