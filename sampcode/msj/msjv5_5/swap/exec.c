/*
   EXEC function with memory swap.
   Needs Assembler file 'spawn.asm'.

Public domain software by

        Thomas Wagner
        Ferrari electronic GmbH
        Beusselstrasse 27
        D-1000 Berlin 21
        West Germany

        BIXname: twagner
*/

#include "compat.h"
#include <ctype.h>

extern int do_exec (char *xfn, char *pars, int spwn, unsigned needed,
						  char **envp);

/*
   The EXEC function.

      Parameters:    xfn   is a string containing the name of the file
                           to be executed. If the string is empty,
                           the COMSPEC environment variable is used to
                           load a copy of COMMAND.COM or its equivalent.
                           If the filename does not include a path, the
                           current PATH is searched after the default.
                           If the filename does not include an extension,
                           the path is scanned for a COM or EXE file in
                           that order.

                     pars  The program parameters.

                     spwn  If 1, the function will return, if necessary
                           after swapping the memory image. 
                           If -1, EMS will not be used when swapping.
                           If 0, the function will terminate after the 
                           EXECed program returns. 
                           NOTE: If the program file is not found, 
                           the function will always return
                           with the appropriate error code, even if 
                           'spwn' is 0.

                     needed   The memory needed for the program in 
                           paragraphs. If not enough memory is free, the
                           program will be swapped out. Use 0 to never
                           swap, 0xffff to always swap. If 'spwn' is false,
                           this parameter is irrelevant.

                     envp  The environment to be passed to the spawned
                           program. If this parameter is NULL, a copy
                           of the parent's environment is used (i.e.
                           'putenv' calls have no effect). If non-NULL,
                           envp must point to an array of pointers to
                           strings, terminated by a NULL pointer (the
                           standard variable 'environ' may be used).

      Return value:
                           0x0000..00FF: The EXECed Program's return code
                           (0..255 decimal)
                           0x0100:       Error writing swap file
                           (256 decimal)
                           0x0200:       Program file not found
                           (512 decimal)
                           0x03xx:       DOS-error-code xx calling EXEC
                           (768..1023 decimal)
                           0x0400:       Error allocating environment buffer
                           (1024 decimal)
*/


#define SWAP_FILENAME "$$AAAAAA.AAA" 

#define M_SWAPPING   0x01
#define M_USE_EMS    0x02
#define M_CREAT_TEMP 0x04
#define M_EXEC       0x80


extern int do_spawn (unsigned char method, 
                     char *swapfn, char *xeqfn, char *cmdtail, 
                     unsigned envlen, char *envp);


int exists (char *fn)

   /* Returns TRUE if a file with name 'fn' exists. */

{
   struct ffblk fb;

   return !findfirst (fn, 
                      &fb, 
                      FA_RDONLY | FA_HIDDEN | FA_SYSTEM | FA_ARCH);
}


int tryext (char *fn)

   /* Try '.COM' and '.EXE' on current filename, modify filename if found. */

{
	int i;

   i = strlen (fn);
   strcat (fn, ".COM");
   if (exists (fn))
      return 1;
   fn [i] = 0;
   strcat (fn, ".EXE");
   if (exists (fn))
      return 1;
   fn [i] = 0;
   return 0;
}


int findfile (char *fn)

   /* Try to find the file 'fn' in the current path. Modifies the filename
      accordingly. */

{
   char path [256];
   char *prfx;
   int found;
   char drive [MAXDRIVE], dir [MAXDIR], name [MAXFILE], ext [MAXEXT];

   if (!fn [0])
      strcpy (fn, getenv ("COMSPEC"));

   fnsplit (fn, drive, dir, name, ext);

   if (ext [0])
      found = exists (fn);
   else
      found = tryext (fn);

   if (!found && !dir [0] && !drive [0])
      {
		strcpy (path, getenv ("PATH"));
      prfx = strtok (path, ";");

      while (!found && prfx != NULL)
         {
         fnmerge (fn, drive, prfx, name, ext);
         if (ext [0])
            found = exists (fn);
         else
            found = tryext (fn);

         prfx = strtok (NULL, ";");
         }
		}
   return found;
}


/*
   tempdir: Set temporary file path.
            Read "TMP/TEMP" environment. If empty or invalid, clear path.
            If TEMP is drive or drive+backslash only, return TEMP.
            Otherwise check if given path is a valid directory.
            If so, add a backslash, else clear path.
*/

void tempdir (char *outfn)
{
	int l;
   char drive [MAXDRIVE], dir [MAXDIR];
	char name [MAXFILE], ext [MAXEXT];
   char *s;
   struct stat ff;
   union REGS regs;
   struct SREGS segregs;

   *outfn = 0;
   if ((s = getenv ("TMP")) == NULL)
      if ((s = getenv ("TEMP")) == NULL)
         return;

   strcpy (outfn, s);
   l = strlen (outfn);
   if (!l)
      return;

   if (outfn [l - 1] == '\\' || outfn [l - 1] == '/')
      outfn [--l] = 0;

   fnsplit (outfn, drive, dir, name, ext);

   if (drive [0])
      {
      regs.h.dl = toupper (drive [0]) - 'A' + 1;
      regs.h.ah = 0x1c;
      intdosx (&regs, &regs, &segregs);
      if (regs.h.al == 0xff)
         {
         *outfn = 0;
         return;
         }
      }

   if (!name [0])   /* No dir name */
      {
      if (dir [0])
         *outfn = 0;
      else
         {
			outfn [l++] = '\\';
         outfn [l] = 0;
         }
      return;
      }

   if (stat (outfn, &ff))
      *outfn = 0;
   else if (!(ff.st_mode & S_IFDIR) || !(ff.st_mode & S_IWRITE))
      *outfn = 0;
   else
      {
      outfn [l++] = '\\';
      outfn [l] = 0;
      }
}


int do_exec (char *exfn, char *epars, int spwn, unsigned needed, char **envp)
{
   char swapfn [80];
   char xfbuf [80];
	char *xfn;
	char pars [130];
   unsigned avail;
   union REGS regs;
   unsigned envlen, rc;
   int idx;
   char **env;
   char *ep, *envbuf;
   unsigned char method;

	xfn = &xfbuf [1];
   strcpy (xfn, exfn);
   strcpy (&pars [1], epars);
   pars [0] = (char)strlen (epars);

   /* First, check if the file to execute exists. */

   if (!findfile (xfn))
      return 0x200;

   /* Now create a copy of the environment if the user wants it. */

   envlen = 0;
   if (envp != NULL)
      for (env = envp; *env != NULL; env++)
         envlen += strlen (*env) + 1;

   if (envlen)
      {
      envlen++;
      ep = envbuf = malloc (envlen);
      if (envbuf == NULL)
         return 0x400;
      for (env = envp; *env != NULL; env++)
         {
         ep = stpcpy (ep, *env) + 1;
         }
      *ep = 0;
      }


	xfbuf [0] = (char)strlen (xfn);

   if (!spwn)
      method = M_EXEC;    /* Mark 'EXEC' function */
   else
      {
      /* Determine amount of free memory */
		regs.x.ax = 0x4800;
		regs.x.bx = 0xffff;
      intdos (&regs, &regs);
		avail = regs.x.bx;

      /* No swapping if available memory > needed */

      if (needed < avail)
         method = 0;
      else
         {
         /* Swapping necessary, use 'TMP' or 'TEMP' environment variable
           to determine swap file path if defined. */

         method = (unsigned char)
                  ((spwn < 0) ? M_SWAPPING : M_SWAPPING | M_USE_EMS);

         swapfn [0] = 1; /* dummy length byte */
         tempdir (&swapfn [1]);

         if (OS_MAJOR >= 3)
            method |= M_CREAT_TEMP;
         else
            {
            strcat (swapfn, SWAP_FILENAME);
            idx = strlen (swapfn) - 1;
            while (exists (swapfn))
               {
               if (swapfn [idx] == 'Z')
                  idx--;
               if (swapfn [idx] == '.')
                  idx--;
               swapfn [idx]++;
               }
            }
         }
		}

   /* All set up, ready to go. */

   rc = do_spawn (method, swapfn, xfbuf, pars, envlen, envbuf);

   /* Free the environment buffer if it was allocated. */

   if (envlen)
      free (envbuf);

   return rc;
}

