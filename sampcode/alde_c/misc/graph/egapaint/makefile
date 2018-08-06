paint.exe: paint.obj mouse.obj phelp.obj lstyle.obj printscr.obj
 tlink .\lib\c0h paint printscr mouse phelp lstyle,paint,,.\lib\mathh .\lib\fp87 .\lib\graphics .\lib\ch

mouse.obj: mouse.c 	
	tcc -mh -1 -c -G -f87 -I.\include -L.\lib mouse.c

paint.obj: paint.c 
	tcc -mh -1 -c -G -f87 -I.\include -L.\lib paint.c

phelp.obj: phelp.c 
	tcc -mh -1 -c -G -f87 -I.\include -L.\lib phelp.c

lstyle.obj: lstyle.c 
	tcc -mh -1 -c -G -f87 -I.\include -L.\lib lstyle.c

printscr.obj: printscr.c
	tcc -mh -1 -c -G -f87 -I.\include -L.\lib printscr.c


