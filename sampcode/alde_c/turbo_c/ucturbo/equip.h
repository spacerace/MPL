/*************************************************************************
 *                                                                       *
 *                            equip.h                                    *
 *                                                                       *
 *    This header defines a structure to hold equipment installed data   *
 *                                                                       *
 *************************************************************************/

struct EQUIP
       {
           int   disk_yn,      /* 1 = yes disks installed                */
                 game_adpt,    /* 1 = game adapter installed             */
                 print_port,   /* number of printer ports installed      */
                 n232,         /* number of serial ports installed       */
                 vmode,        /* initial video mode                     */
                 ndisks,       /* number of disk drives installed        */
                 mem;          /* amount of base memory installed        */
       };
