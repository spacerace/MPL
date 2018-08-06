
#define INCL_WIN
#include <os2.h>

extern MRESULT EXPENTRY ClientWndProc ( long near *, int
near *, long near *, long near *);

MRESULT EXPENTRY BasClientWndProc(HWND hwnd, USHORT msg,
MPARAM mp1, MPARAM mp2)

{
   static long near hwndb;
   static int near  msgb;
   static long near mp1b;
   static long near mp2b;
   hwndb=(long)hwnd;
   msgb=(int)msg;
   mp1b=(long)mp1;
   mp2b=(long) mp2;
   return ClientWndProc(&hwndb, &msgb, &mp1b, &mp2b);

} /* BasClientWndProc */


long pascal far RegBas(void)
{
   return (long) BasClientWndProc;

} /* regbas */


long pascal Makelong (long passedlong)
{
   return passedlong;
} /* MakeLong */


void pascal BreakLong(int hiword, int loword, int *phiword,
int *ploword)
{
   *phiword = hiword;
   *ploword = loword;
} /* BreakLong */

