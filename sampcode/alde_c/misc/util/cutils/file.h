/*
                AUTHOR          Jon Wesener
                DATE             7 / 8/ 85
                PURPOSE           This is the include file to be
                                used with the Lattice library's
                                file manipulation commands
*/
 
/* Create File Attributes */

#define		FC$NORM		0	/* NORMAL */ 
#define         FC$RD_O         1       /* READ ONLY */
#define         FC$HID          2       /* HIDDEN */
#define         FC$SYS          4       /* SYSTEM */
 
/* Open File Accesses */
 
#define         FO$READ         0       /* READ */
#define         FO$WRTE         1       /* WRITE */
#define         FO$UPDT         2       /* UPDATE */
 
/* Fmove Bases */
 
#define         FM$BOF          0       /* BEGINNING OF FILE */
#define         FM$CUR          1       /* CURRENT POSITION IN FILE */
#define         FM$EOF          2       /* END OF FILE */

/* File Read and Write Channels */

#define         STDIN           0
#define         STDOUT          1
#define         STDERR          2