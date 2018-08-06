/*
   TSKBUF.C - CTask - Buffered Message handling routines.

   Public Domain Software written by
      Thomas Wagner
      Patschkauer Weg 31
      D-1000 Berlin 33
      West Germany

   NOTE: None of the Buffer routines my be used from inside an interrupt
         handler. The routines are relatively slow, since they use
         the normal word pipe and resource calls. Optimization would be 
         possible by using block moves and internally handling resources.
*/

#include <stdio.h>

#include "tsk.h"
#include "tsklocal.h"


bufferptr far create_buffer (bufferptr buf, farptr pbuf, word bufsize
#if (TSK_NAMEPAR)
                        ,byteptr name
#endif
                        )
{
#if (TSK_DYNAMIC)
   if (buf == NULL)
      {
      if ((buf = tsk_alloc (sizeof (buffer))) == NULL)
         return NULL;
      buf->flags = F_TEMP;
      }
   else
      buf->flags = 0;
   if (pbuf == NULL)
      {
      if ((pbuf = tsk_alloc (bufsize)) == NULL)
         {
         if (buf->flags & F_TEMP)
            tsk_free (buf);
         return NULL;
         }
      buf->flags |= F_STTEMP;
      }
#endif

   create_wpipe (&buf->pip, pbuf, bufsize
#if (TSK_NAMEPAR)
                 ,name
#endif
                 );
   create_resource (&buf->buf_read
#if (TSK_NAMEPAR)
                 ,name
#endif
                 );
   create_resource (&buf->buf_write
#if (TSK_NAMEPAR)
                 ,name
#endif
                 );
   buf->msgcnt = 0;

#if (TSK_NAMED)
   tsk_add_name (&buf->name, name, TYP_BUFFER, buf);
#endif

   return buf;
}


void far delete_buffer (bufferptr buf)
{
   delete_wpipe (&buf->pip);
   delete_resource (&buf->buf_read);
   delete_resource (&buf->buf_write);
   buf->msgcnt = 0;

#if (TSK_NAMED)
   tsk_del_name (&buf->name);
#endif

#if (TSK_DYNAMIC)
   if (buf->flags & F_STTEMP)
      tsk_free (buf->pip.wcontents);
   if (buf->flags & F_TEMP)
      tsk_free (buf);
#endif
}


int far read_buffer (bufferptr buf, farptr msg, int size, dword timeout)
{
   int i, len, l1, l2;
   word w;

   if ((i = request_resource (&buf->buf_read, timeout)) < 0)
      return i;

   if ((len = read_wpipe (&buf->pip, timeout)) < 0)
      {
      release_resource (&buf->buf_read);
      return len;
      }

   l1 = (len < size) ? len : size;

   for (i = 0; i < l1; i++)
      {
      if (!(i & 1))
         w = read_wpipe (&buf->pip, 0L);
      else
         w = w >> 8;
      ((byteptr)msg) [i] = (byte)w;
      }

   l2 = (len + 1) >> 1;
   for (i = (l1 + 1) >> 1; i < l2; i++)
      read_wpipe (&buf->pip, 0L);
   if (l1 < size)
      ((byteptr)msg) [l1] = 0;

   buf->msgcnt--;

   release_resource (&buf->buf_read);
   return len;
}


int far c_read_buffer (bufferptr buf, farptr msg, int size)
{
   int res;
   CRITICAL;

   res = -1;
   C_ENTER;

   if (buf->pip.filled && buf->buf_read.state)
      {
      request_resource (&buf->buf_read, 0L);
      C_LEAVE;
      res = read_buffer (buf, msg, size, 0L);
      }
   else
      C_LEAVE;

   return res;
}


local int near tsk_wrbuf (bufferptr buf, word w, dword timeout)
{
   int i;

   if ((i = write_wpipe (&buf->pip, w, timeout)) < 0)
      release_resource (&buf->buf_write);
   return i;
}


int far write_buffer (bufferptr buf, farptr msg, int size, dword timeout)
{
   int i, res, l2;

   if (size < 0 || (word)((size + 3) >> 1) > buf->pip.bufsize)
      return -3;

   if ((i = request_resource (&buf->buf_write, timeout)) < 0)
      return i;

   if ((i = tsk_wrbuf (buf, (word)size, timeout)) < 0)
      return i;

   l2 = (size + 1) >> 1;

   for (i = 0; i < l2; i++)
      if ((res = tsk_wrbuf (buf, ((wordptr)msg) [i], timeout)) < 0)
         return res;

   buf->msgcnt++;

   release_resource (&buf->buf_write);
   return size;
}


int far c_write_buffer (bufferptr buf, farptr msg, int size)
{
   int res;
   CRITICAL;

   if (size < 0 || (word)((size + 3) >> 1) > buf->pip.bufsize)
      return -3;

   C_ENTER;
   res = -1;

   if (wpipe_free (&buf->pip) >= ((size + 3) >> 1) && buf->buf_write.state)
      {
      request_resource (&buf->buf_write, 0L);
      C_LEAVE;
      res = write_buffer (buf, msg, size, 0L);
      }
   else
      C_LEAVE;

   return res;
}


word far check_buffer (bufferptr buf)
{
   return buf->msgcnt;
}

