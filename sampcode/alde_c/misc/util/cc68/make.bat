cl -c analyze.c expr.c init.c optimize.c register.c func.c
cl -c intexpr.c outcode.c searchkw.c cglbdef.c gencode.c list.c
cl -c peepgen.c stmt.c cmain.c genstmt.c memmgt.c preproc.c
cl -c symbol.c decl.c getsym.c
cl -o cc68 *.obj

