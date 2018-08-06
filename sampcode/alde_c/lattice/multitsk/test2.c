int conlock;
int *comanch;
struct INPUT
   { struct INPUT *next;
     char text[2];
   };
struct INPUT *diskqueue;
char *getmem();

/*  This program tests the multi-tasking functions.
*   Three functions are attached:
*       1.  keytask, which just echoes characters from the keyboard
*           to the console until a tilde (~) is encountered.
*       2.  diskin, which copies the file "taskfunc.asm" to an internal
*           queue, one full line at a time.
*       3.  diskout, which dumps the internal queue, at most 10 chars
*           at a time, to the file "test2.out"
*
*   The main task just waits until all subtasks have completed.
*
*/
#include "stdio.h"

main()
{
  int j;
  int i;


  inittask("main");
  if (attach(getmem(100),getmem(1024),1024)) keytask();
  if (attach(getmem(100),getmem(1024),1024)) diskin();
  if (attach(getmem(100),getmem(1024),1024)) diskout();
  while (taskcnt() > 1) { yield(); }
}

keytask()
{
  char c;


printf("\nKeyboard task activated");
for (;;)

   {
     if (iskbhit())
       {
        c = getch();
        if (c == '\r') putch('\n');
        putch(c);
        if (c == '~')
          {
             printf("\nKeyboard task terminating");
             stop();
          }
       }
     yield();
   }
}

diskin()
{
  FILE *fp;
  char buffer[256], *sp;
  struct INPUT *nq, *wp;

  printf("\nRead task now active");

  fp = fopen("taskfunc.asm","r");
  if (!fp)  {printf("\nUnable to open taskfunc.asm - terminating"); exit(3);}
  while ((sp = fgets(buffer,256,fp)))
    {
      nq = (struct INPUT *) getmem( 8+sizeof(char *) + strlen(sp));
      nq->next = NULL;
      strcpy(nq->text, sp);
      wp = (struct INPUT *) &diskqueue;
      while (wp->next) { wp = wp->next; }
      wp->next = nq;
      yield();
    }
  fclose(fp);
  printf("\n\nRead subtask terminated.");
  stop();
}
diskout()
{
  FILE *fp;
  char *sp;
  int j;
  struct INPUT *nq, *wp;
  char c;

  fp = fopen("test2.out","w");
  printf("\nWritetask now active");
  while ((nq=diskqueue) || (taskcnt() > 3))
    {
      diskqueue = nq->next;
      sp = nq->text;
      j=0;
      while (*sp)
        { fputc(*sp++, fp);
          if (j++>10) {yield(); j=0;}
        }
      rlsmem(nq,8+sizeof(char *) + strlen(nq->text));

      yield();
    }
  printf("\n\nWrite task terminating");
  fclose(fp);
  stop();
}
