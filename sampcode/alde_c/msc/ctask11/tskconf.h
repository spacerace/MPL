/*
   TSKCONF.H - CTask - Configuration definitions 

   Public Domain Software written by
      Thomas Wagner
      Patschkauer Weg 31
      D-1000 Berlin 33
      West Germany

   This file is new with Version 1.1
*/

/*
   TSK_DYNAMIC    if 1, creating dynamic control blocks is allowed.

   CAUTION:    Changing this define requires changing the corresponding
               definition in TSK.MAC and recompilation/assembly of *all*
               sources.
*/

#define  TSK_DYNAMIC    1

/*
   TSK_NAMEPAR    if 1, the create_xxx routines accept a name parameter.

*/

#define  TSK_NAMEPAR    1

/*
   TSK_NAMED      if 1, control block names are processed.

   CAUTION:    Changing this define requires changing the corresponding
               definition in TSK.MAC and recompilation/assembly of *all*
               sources.
               TSK_NAMED may only be enabled (1) if TSK_NAMEPAR is also 
               enabled.
*/

#if (TSK_NAMEPAR)

#define  TSK_NAMED      1     /* This may be set to 0 if desired */

#else
#define  TSK_NAMED      0     /* Do not change this */
#endif

/*
   CLOCK_MSEC     if 1, all timeouts are specified in milliseconds.
                  Requires floating point library.
                  Changing requires recompile of tskmain.c and tsksub.c.
*/

#define CLOCK_MSEC      0

/*
   PRI_TIMER      Priority of the timer task. Normally higher than any
                  other task in the system.
                  Changing requires recompile of tskmain.c.
*/

#define  PRI_TIMER   0xf000

/*
   PRI_STD        standard priority of user tasks (arbitrary), for use
                  in user programs.
                  Changing requires recompile of tskmain.c.
*/

#define  PRI_STD     100

/*
   PRI_INT9       priority of the int9 task. Normally lower than or equal to
                  user defined tasks, but may be adjusted to tune responses
                  of TSR's.
                  Changing requires recompile of tskmain.c.
*/

#define  PRI_INT9    (PRI_STD - 1)

/*
   IBM            if 1, routines specific to the IBM family of PCs
                  (and clones) are installed.

   DOS            if 1, routines specific to PC/MS-DOS are installed.

      NOTE: If you disable IBM, you have to provide timer and keyboard
            hardware support yourself. Check the areas in "tskmain.c" 
            that are affected by this define.
            Changing requires recompile of tskmain.c.
*/

#define  IBM   1
#define  DOS   1


/*
   AT_BIOS        if 1, the AT BIOS wait/post handler is installed.
                  Changing requires recompile of tskmain.c.
*/

#define  AT_BIOS  1

