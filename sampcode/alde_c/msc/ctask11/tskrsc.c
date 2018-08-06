/*
   TSKRSC.C - CTask - Resource handling routines.

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
   create resource - initialise resource.
*/

resourceptr far create_resource (resourceptr rsc
#if (TSK_NAMEPAR)
                                 ,byteptr name
#endif
                                 )
{
#if (TSK_DYNAMIC)
   if (rsc == NULL)
      {
      if ((rsc = tsk_alloc (sizeof (resource))) == NULL)
         return NULL;
      rsc->flags = F_TEMP;
      }
   else
      rsc->flags = 0;
#endif

   rsc->waiting = NULL;
   rsc->state = 1;
   rsc->owner = NULL;

#if (TSK_NAMED)
   tsk_add_name (&rsc->name, name, TYP_RESOURCE, rsc);
#endif

   return rsc;
}

/*
   delete_resource - Kill all tasks waiting for the resource.
*/

void far delete_resource (resourceptr rsc)
{
   CRITICAL;

   C_ENTER;
   tsk_kill_queue (&(rsc->waiting));
   rsc->owner = NULL;
   rsc->state = 0;
   C_LEAVE;

#if (TSK_NAMED)
   tsk_del_name (&rsc->name);
#endif

#if (TSK_DYNAMIC)
   if (rsc->flags & F_TEMP)
      tsk_free (rsc);
#endif
}


/*
   request_resource - Wait until resource is available. If the resource
                      is free on entry, it is assigned to the task, and
                      the task continues to run.
                      If the task requesting the resource already owns it,
                      this routine returns 0.
*/

int far request_resource (resourceptr rsc, dword timeout)
{
   CRITICAL;

   C_ENTER;
   if (rsc->state || rsc->owner == tsk_current)
      {
      rsc->state = 0;
      rsc->owner = tsk_current;
      C_LEAVE;
      return 0;
      }
   tsk_current->retptr = NULL;
   tsk_wait (&rsc->waiting, timeout);
   return (int)tsk_current->retptr;
}


/*
   c_request_resource - If the resource is free on entry, it is assigned 
                        to the task, otherwise an error status is returned.
*/

int far c_request_resource (resourceptr rsc)
{
   int rv;
   CRITICAL;

   C_ENTER;
   if (rv = rsc->state)
      {
      rsc->state = 0;
      rsc->owner = tsk_current;
      }
   C_LEAVE;
   return (rv) ? 0 : -1;
}


/*
   release_resource - Release the resource. If there are tasks waiting for
                      the resource, it is assigned to the first waiting
                      task, and the task is made eligible.
*/

void far release_resource (resourceptr rsc)
{
   tcbptr curr;
   CRITICAL;

   if (rsc->owner != tsk_current)
      return;

   C_ENTER;
   if ((curr = rsc->waiting) == NULL)
      {
      rsc->state = 1;
      rsc->owner = NULL;
      C_LEAVE;
      return;
      }

   rsc->waiting = tsk_runable (curr);
   rsc->owner = curr;
   C_LEAVE;
}


/*
   check_resource - returns 1 if the resource is available.
*/

int far check_resource (resourceptr rsc)
{
   return rsc->state;
}

