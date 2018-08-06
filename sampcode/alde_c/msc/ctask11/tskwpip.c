/*
   TSKWPIP.C - CTask - Word Pipe handling routines.

   Public Domain Software written by
      Thomas Wagner
      Patschkauer Weg 31
      D-1000 Berlin 33
      West Germany
*/

#include <stdio.h>

#include "tsk.h"
#include "tsklocal.h"

/*
   tsk_getwpipe - get a word from a pipe. For internal use only.
                  Critical section assumed entered.
*/

local word near tsk_getwpipe (wpipeptr pip)
{
   word c;

   c = pip->wcontents [pip->outptr++];
   if (pip->outptr >= pip->bufsize)
      pip->outptr = 0;
   pip->filled--;
   return c;
}


/*
   tsk_putwpipe - put a word to a pipe. For internal use only.
                  Critical section assumed entered.
*/

local void near tsk_putwpipe (wpipeptr pip, word c)
{
   pip->wcontents [pip->inptr++] = c;
   if (pip->inptr >= pip->bufsize)
      pip->inptr = 0;
   pip->filled++;
}


/* -------------------------------------------------------------------- */


/*
   create_wpipe - initialises wpipe.
*/

wpipeptr far create_wpipe (wpipeptr pip, farptr buf, word bufsize
#if (TSK_NAMEPAR)
                           ,byteptr name
#endif
                           )
{
#if (TSK_DYNAMIC)
   if (pip == NULL)
      {
      if ((pip = tsk_alloc (sizeof (wpipe))) == NULL)
         return NULL;
      pip->flags = F_TEMP;
      }
   else
      pip->flags = 0;
   if (buf == NULL)
      {
      if ((buf = tsk_alloc (bufsize)) == NULL)
         {
         if (pip->flags & F_TEMP)
            tsk_free (pip);
         return NULL;
         }
      pip->flags |= F_STTEMP;
      }
#endif

   pip->wait_read = pip->wait_write = pip->wait_clear = NULL;
   pip->outptr = pip->inptr = pip->filled = 0;
   pip->bufsize = bufsize >> 1;
   pip->wcontents = (wordptr)buf;

#if (TSK_NAMED)
   tsk_add_name (&pip->name, name, TYP_WPIPE, pip);
#endif

   return pip;
}


/*
   delete_wpipe - kills all processes waiting for reading from or writing
                  to the wpipe.
*/

void far delete_wpipe (wpipeptr pip)
{
   CRITICAL;

   C_ENTER;
   tsk_kill_queue (&(pip->wait_read));
   tsk_kill_queue (&(pip->wait_write));
   tsk_kill_queue (&(pip->wait_clear));
   pip->outptr = pip->inptr = pip->filled = 0;
   C_LEAVE;

#if (TSK_NAMED)
   tsk_del_name (&pip->name);
#endif

#if (TSK_DYNAMIC)
   if (pip->flags & F_STTEMP)
      tsk_free (pip->wcontents);
   if (pip->flags & F_TEMP)
      tsk_free (pip);
#endif
}


/*
   read_wpipe - Wait until a word is written to the wpipe. If there 
                is a word in the wpipe on entry, it is assigned to 
                the caller, and the task continues to run. If there are
                tasks waiting to write, the first task is made eligible,
                and the word is inserted into the wpipe.
*/

word far read_wpipe (wpipeptr pip, dword timeout)
{
   tcbptr curr;
   word res;
   CRITICAL;

   C_ENTER;

   if (pip->filled)
      {
      res = tsk_getwpipe (pip);

      if ((curr = pip->wait_write) != NULL)
         {
         tsk_putwpipe (pip, curr->retsize);
         pip->wait_write = tsk_runable (curr);
         curr->retptr = NULL;
         }
      else if (!pip->filled)
         while (pip->wait_clear != NULL)
            pip->wait_clear = tsk_runable (pip->wait_clear);

      C_LEAVE;
      return res;
      }

   tsk_wait (&pip->wait_read, timeout);
   return (word)tsk_current->retptr;
}


/*
   c_read_wpipe - If there is a word in the wpipe on entry,
                  read_wpipe is called, otherwise an error status is returned.
*/

word far c_read_wpipe (wpipeptr pip)
{
   CRITICAL, res;

   C_ENTER;
   res = (pip->filled) ? read_wpipe (pip, 0L) : (word)-1;
   C_LEAVE;
   return res;
}



/*
   write_wpipe - Wait until space for the word to be written to the 
                 wpipe is available. If there is enough space in the wpipe 
                 on entry, the word is inserted into the wpipe, and
                 the task continues to run. If there are tasks waiting 
                 to read, the first task is made eligible, and the word
                 is passed to the waiting task.
*/

int far write_wpipe (wpipeptr pip, word ch, dword timeout)
{
   tcbptr curr;
   CRITICAL;

   C_ENTER;

   if (pip->filled < pip->bufsize)
      {
      if ((curr = pip->wait_read) != NULL)
         {
         pip->wait_read = tsk_runable (curr);
         curr->retptr = (farptr)ch;
         C_LEAVE;
         return 0;
         }

      tsk_putwpipe (pip, ch);
      C_LEAVE;
      return 0;
      }

   tsk_current->retsize = ch;
   tsk_wait (&pip->wait_write, timeout);
   return (int)tsk_current->retptr;
}


/*
   c_write_wpipe - If there is space for the word in the wpipe on entry,
                   write_wpipe is called, otherwise an error status is returned.
*/

int far c_write_wpipe (wpipeptr pip, word ch)
{
   int res;
   CRITICAL;

   C_ENTER;
   res = (pip->filled < pip->bufsize) ? write_wpipe (pip, ch, 0L) : -1;
   C_LEAVE;
   return res;
}


/*
   wait_wpipe_empty - Wait until the pipe is empty. If the pipe is
                      empty on entry, the task continues to run.
*/

int far wait_wpipe_empty (wpipeptr pip, dword timeout)
{
   CRITICAL;

   C_ENTER;
   if (!pip->filled)
      {
      C_LEAVE;
      return 0;
      }

   tsk_current->retptr = NULL;
   tsk_wait (&pip->wait_clear, timeout);
   return (int)tsk_current->retptr;
}


/*
   check_wpipe - returns -1 if there are no words in the wpipe, else
                 the first available word.
*/

word far check_wpipe (wpipeptr pip)
{
   return (pip->filled) ? pip->wcontents [pip->outptr] : (word)-1;
}


/*
   wpipe_free - returns the number of free words in the pipe.
*/

word far wpipe_free (wpipeptr pip)
{
   return pip->bufsize - pip->filled;
}


