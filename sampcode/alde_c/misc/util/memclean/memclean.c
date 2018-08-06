/*

        MEMORY CLEAN PROGRAM FOR THE IBM PERSONAL COMPUTER
        Version 1.00  3 May 1983

        COPYRIGHT (C) ROBERT J. BEILSTEIN, 1983

                      N O T I C E

      This program is supplied for your personal and
      non-commercial use only. No commercial use permitted.
      Users of this program are granted limited rights to
      reproduce and distribute this program, provided
      the following conditions are met:

      1) This program must be supplied in original form,
         with no modifications.
      2) Notices contained in this source file, and those
         produced by the executable program, must not be
         altered or removed.
      3) This program may not be sold. Providing this
         program for consideration of any sort, including
         copying or distribution fees, is prohibited.
      4) This program may not be used on timesharing
         systems where fees are charged for access,
         or or supplied as part of any other form of
         rental access computing, without the express
         written permission of the author.

      PLEASE SEND PROBLEM REPORTS AND SUGGESTIONS
      TO:
            ROBERT J. BEILSTEIN
            413 Wells Avenue, West
            North Syracuse, New York 13212

        This program is designed to be run immediately following
        powerup, and will fill all of expansion memory with good-parity
        data (thus avoiding problems with spurious parity checks
        if uninitialized memory is read).

        The program will write to all available memory locations
        above the 640K the ROM BIOS knows about.  That is:

        X'C0000' --> X'EFFFF'

        In addition, if 'SEGA' is specified on the command line,
        locations X'A0000' --> X'AFFFF' will be initialized.

        Also, for compatibility with pre-XT machines which have
        a limit on DOS-addressible RAM of 544K, specifying '544K'
        on the command line will start clearing at X'88000'.

*/

#include <stdio.h>

#define WRITE_DATA 0xaaaa   /* memory initialization value */

main(argc,argv)     /* define entry point */

int argc;           /* count of function arguments */
char *argv[];       /* pointer array to argument strings */
{
    unsigned int cur_segment;           /* current segment pointer */
    unsigned int cur_offset;            /* current offset within segment */
    unsigned int start_segment=0xa000;  /* starting segment */
    unsigned int start_offset=0;        /* starting offset within segment
                                           used to start 544k seg on 32k
                                           boundary */

                                        /* a bunch of useful flags */
        unsigned is_xt = 1;             /* if set, start at 640k */
        unsigned resv_ok = 0;           /* if set, clear reserved memory
                                           between X'A0000' and X'AFFFF' */
        unsigned no_high = 0;           /* if set, do not initialize
                                           X'C0000' or above */
        unsigned good_parm;             /* current parameter valid */

    int i;                              /* the canonical temporary */

    unsigned long bot,top;              /* for displaying limit values */

    printf("\nMEMCLEAN V1.00, COPYRIGHT (C) Robert J. Beilstein, 1983\n\n");

    for (i = 1;i < argc;i++)            /* check invocation parms */
    {
        good_parm = 0;                  /* reset good parm flag */

        if (!strcmp(argv[i],"544k") && strcmp(argv[i],"544K"))
        {
            is_xt = 0;                  /* start at X'88000' */

            good_parm = 1;
        }

        if (!strcmp(argv[i],"sega") && strcmp(argv[i],"SEGA"))
        {

            resv_ok = 1;                /* do X'A0000' --> X'AFFFF' */

            good_parm = 1;

        }

        if (!strcmp(argv[i],"nohigh") && strcmp(argv[i],"NOHIGH"))
        {

            no_high = 1;                /* skip X'C0000' --> X'EFFFF'  */

            good_parm = 1;

        }

        if (!good_parm)                 /* execute if a bad parm given */
            abort("INVALID PARAMETER TO <MEMCLEAN>:  \"%s\"\nVALID PARAMETERS\
 ARE: \"544k\", \"sega\" and \"nohigh\"",argv[i]);
    }

    /* now that parameters are decoded, we can start doing something useful */

    if (!is_xt)                         /* if "544K" specified, reset addr */
    {

        start_segment = 0x8000;
        start_offset = 0x8000;

    }

    for (cur_segment = start_segment;cur_segment < 0xf000;cur_segment+=4096)
    {

        if ((!resv_ok) && cur_segment == 0xa000) continue;

        if (cur_segment == 0xb000) continue;    /* skip graphics ram */

        if (no_high && cur_segment >= 0xc000) break;    /* quit on "nohigh */

        bot = (unsigned long) cur_segment * 16 + start_offset;
        top = (unsigned long) cur_segment * 16 + 65535;

        printf("INITIALIZING %lx --> %lx\n",bot,top);   /* show same */

        for (cur_offset = start_offset;;cur_offset+=2)
        {
            pokew(cur_offset,cur_segment,WRITE_DATA);

            if (cur_offset == 0xfffe) break;

        }

        start_offset = 0;               /* subsequently start at offset 0 */

    }

    printf("ALL REQUESTED MEMORY INITIALIZED\n");

}
    powerup, and will fill alluently start at offset 0 */

    }

    printf("ALL REQUESTED MEMORY INITIALIZED\n");

}
    powerup, and will fill all