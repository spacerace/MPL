/*
   TSKPIP.C - CTask - Pipe handling routines.

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
   tsk_getcpipe - get a byte from a pipe. For internal use only.
                  Critical section assumed entered.
*/

local byte near tsk_getcpipe (pipeptr pip)
{
   byte c;

   c = pip->contents [pip->outptr++];
   if (pip->outptr >= pip->bufsize)
      pip->outptr = 0;
   pip->filled--;
   return c;
}


/*
   tsk_putcpipe - put a byte to a pipe. For internal use only.
                  Critical section assumed entered.
*/

local void near tsk_putcpipe (pipeptr pip, byte c)
{
   pip->contents [pip->inptr++] = c;
   if (pip->inptr >= pip->bufsize)
      pip->inptr = 0;
   pip->filled++;
}


/* -------------------------------------------------------------------- */


/*
   create_pipe - initialises pipe.
*/

pipeptr far create_pipe (pipeptr pip, farptr buf, word bufsize
#if (TSK_NAMEPAR)
                        ,byteptr name
#endif
                        )
{
#if (TSK_DYNAMIC)
   if (pip == NULL)
      {
      if ((pip = tsk_alloc (sizeof (pipe))) == NULL)
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
   pip->bufsize = bufsize;
   pip->contents = (byteptr)buf;

#if (TSK_NAMED)
   tsk_add_name (&pip->name, name, TYP_PIPE, pip);
#endif

   return pip;
}


/*
   delete_pipe - kills all processes waiting for reading from or writing
                 to the pipe.
*/

void far delete_pipe (pipeptr pip)
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
      tsk_free (pip->contents);
   if (pip->flags & F_TEMP)
      tsk_free (pip);
#endif
}


/*
   read_pipe - Wait until a character is written to the pipe. If there 
               is a character in the pipe on entry, it is assigned to 
               the caller, and the task continues to run. If there are
               tasks waiting to write, the first task is made eligible,
               and the character is inserted into the pipe.
*/

int far read_pipe (pipeptr pip, dword timeout)
{
   tcbptr curr;
   int res;
   CRITICAL;

   C_ENTER;

   if (pip->filled)
      {
      res = tsk_getcpipe (pip);

      if ((curr = pip->wait_write) != NULL)
         {
         tsk_putcpipe (pip, (byte)curr->retsize);
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
   return (int)tsk_current->retptr;
}


/*
   c_read_pipe - If there is a character in the pipe on entry,
                 read_pipe is called, otherwise en error status is returned.
*/

int far c_read_pipe (pipeptr pip)
{
   CRITICAL, res;

   C_ENTER;
   res = (pip->filled) ? read_pipe (pip, 0L) : -1;
   C_LEAVE;
   return res;
}



/*
   write_pipe - Wait until space for the character to be written to the 
                pipe is available. If there is enough space in the pipe 
                on entry, the character is inserted into the pipe, and
                the task continues to run. If there are tasks waiting 
                to read, the first task is made eligible, and the character
                is passed to the waiting task.
*/

int far write_pipe (pipeptr pip, byte ch, dword timeout)
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

      tsk_putcpipe (pip, ch);
      C_LEAVE;
      return 0;
      }

   tsk_current->retsize = ch;
   tsk_wait (&pip->wait_write, timeout);
   return (int)tsk_current->retptr;
}


/*
   c_write_pipe - If there is space for the character in the pipe on entry,
                  write_pipe is called, otherwise en error status is returned.
*/

int far c_write_pipe (pipeptr pip, byte ch)
{
   int res;
   CRITICAL;

   C_ENTER;
   res = (pip->filled < pip->bufsize) ? write_pipe (pip, ch, 0L) : -1;
   C_LEAVE;
   return res;
}


/*
   wait_pipe_empty - Wait until the pipe is empty. If the pipe is
                     empty on entry, the task continues to run.
*/

int far wait_pipe_empty (pipeptr pip, dword timeout)
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
   check_pipe - returns -1 if there are no characters in the pipe, else
                the first available character.
*/

int far check_pipe (pipeptr pip)
{
   return (pip->filled) ? (int)pip->contents [pip->outptr] : -1;
}


/*
   pipe_free - returns the number of free characters in the pipe.
*/

word far pipe_free (pipeptr pip)
{
   return pip->bufsize - pip->filled;
}


