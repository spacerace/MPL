/* SYSINFO.C illustrates miscellaneous DOS and BIOS status functions
 * including:
 *      _dos_getdrive       _dos_setdrive       _dos_getdiskfree
 *      _bios_memsize       _bios_equiplist     _bios_printer
 *
 * See DISK.C for another example of _dos_getdiskfree.
 *
 * Also illustrated:
 *      union               bitfield struct
 */

#include <dos.h>
#include <bios.h>
#include <conio.h>
#include <stdio.h>
#define LPT1 0

main()
{
    struct diskfree_t drvinfo;
    unsigned drive, drivecount, memory, pstatus;
    union
    {                                   /* Access equiment either as:    */
        unsigned u;                     /*   unsigned or                 */
        struct                          /*   bit fields                  */
        {
            unsigned diskflag : 1;      /* Diskette drive installed?     */
            unsigned coprocessor : 1;   /* Coprocessor? (except on PC)   */
            unsigned sysram : 2;        /* Ram on system board           */
            unsigned video : 2;         /* Startup video mode            */
            unsigned disks : 2;         /* Drives 00=1, 01=2, 10=3, 11=4 */
            unsigned dma : 1;           /* 0=Yes, 1=No (1 for PC Jr.)    */
            unsigned comports : 3;      /* Serial ports                  */
            unsigned game : 1;          /* Game adapter installed?       */
            unsigned modem : 1;         /* Internal modem?               */
            unsigned printers : 2;      /* Number of printers            */
        } bits;
    } equip;
    static char y[] = "YES", n[] = "NO";

    /* Get current drive. */
    _dos_getdrive( &drive );
    printf( "Current drive:\t\t\t%c:\n", 'A' + drive - 1 );

    /* Set drive to current drive in order to get number of drives. */
    _dos_setdrive( drive, &drivecount );

    _dos_getdiskfree( drive, &drvinfo );
    printf( "Disk space free:\t\t%ld\n",
            (long)drvinfo.avail_clusters *
            drvinfo.sectors_per_cluster *
            drvinfo.bytes_per_sector );

    /* Get new drive and number of logical drives in system. */
    _dos_getdrive( &drive );
    printf( "Number of logical drives:\t%d\n", drivecount );

    memory = _bios_memsize();
    printf( "Memory:\t\t\t\t%dK\n", memory );

    equip.u = _bios_equiplist();

    printf( "Disk drive installed:\t\t%s\n", equip.bits.diskflag ? y : n );
    printf( "Coprocessor installed:\t\t%s\n", equip.bits.coprocessor ? y : n );
    printf( "Game adapter installed:\t\t%s\n", equip.bits.game ? y : n );
    printf( "Serial ports:\t\t\t%d\n", equip.bits.comports );
    printf( "Number of printers:\t\t%d\n", equip.bits.printers );

    /* Fail if any error bit is on, or if either operation bit is off. */
    pstatus = _bios_printer( _PRINTER_STATUS, LPT1, 0 );
    if( (pstatus & 0x29) || !(pstatus & 0x80) || !(pstatus & 0x10) )
        pstatus = 0;
    else
        pstatus = 1;
    printf( "Printer available:\t\t%s\n", pstatus ? y : n );
}
