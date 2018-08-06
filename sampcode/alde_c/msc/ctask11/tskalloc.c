/*
   TSKALLOC.C - CTask - Dynamic memory allocation interface

   Public Domain Software written by
      Thomas Wagner
      Patschkauer Weg 31
      D-1000 Berlin 33
      West Germany

   This file is new with Version 1.1

   This module contains the memory allocation functions that are needed
   if TSK_DYNAMIC is defined.

*/

#include "tsk.h"

#if (TURBO)
#include <alloc.h>
#else
#include <malloc.h>
#endif

#define xalloc(size) malloc (size)
#define xfree(item)  free (item)

resource alloc_resource;

farptr tsk_alloc (word size)
{
   farptr ptr;

   request_resource (&alloc_resource, 0L);
   ptr = xalloc (size);
   release_resource (&alloc_resource);

   return ptr;
}


void tsk_free (farptr item)
{
   request_resource (&alloc_resource, 0L);
   xfree (item);
   release_resource (&alloc_resource);
}

