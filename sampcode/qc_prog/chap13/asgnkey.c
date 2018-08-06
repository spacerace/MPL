/*   asgnkey.c -- uses ansi.sys to assign meanings   */
/*                to several function keys.          */
/*  Note: This requires ANSI.SYS to be installed.    */

#define KASSIGN(K, S) printf("\033[0;%d;\"%s\";13p", K, S)
/* This macro assigns string S to key K */
#define F5 63
#define F6 64
#define F7 65
#define F8 66
#define F9 67
#define F10 68
main()
{
     KASSIGN(F5, "DIR *.C");
     KASSIGN(F6, "DIR *.H");
     KASSIGN(F7, "DIR *.OBJ");
     KASSIGN(F8, "DIR *.EXE");
     KASSIGN(F9, "DIR /W");
     KASSIGN(F10, "CD \\");
}
