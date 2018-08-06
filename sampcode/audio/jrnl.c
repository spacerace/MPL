/* jrnl.c -
**
**     Sample program that reads TOC from CD and asks CDROM
**     drive to play contents of audio disc.
**     Plays only from the first CDROM drive that it finds.
**
**     Companion documentation:
**             - MSCDEX Function Requests
**               For finding device drivers from MSCDEX
**             - CDROM Device Driver Specification
**               For communicating with the physical device
**             - MSDOS Programmers Reference
**               Documentation on device drivers and device driver layout
*/

#include <stdio.h>
#include <ctype.h>
#include <dos.h>
#include <process.h>

       /* If WHOLE is defined, play entire disc.
       ** If not defined, play about 10 seconds from each audio track
       */
#define WHOLE 1

       /* Macros for building/taking apart long's */
#define MAKELONG(lo, hi)  ((long)(((unsigned)(lo)) | ((unsigned long)((unsigned)(hi))) << 16))
#define LOWORD(l)       ((ushort)(l))
#define HIWORD(l)       ((ushort)(((ulong)(l) >> 16) & 0xffff))
#define LOBYTE(w)       ((uchar)(w))
#define HIBYTE(w)       (((ushort)(w) >> 8) & 0xff)

       /* Addressing modes */
#define        ADDR_HSG        0
#define        ADDR_RED        1

       /* Device driver commands */
#define        DEVRDIOCTL       3      /* IOCTL read                   */
#define        DEVPLAY         132     /* Device Play                  */
#define        DEVSTOP         133     /* Stop device play             */

       /* CDROM Device IOCTL commands */
#define        IOI_ret_addr            0
#define        IOI_loc_head            1
#define        IOI_io_query            2
#define        IOI_err_stats           3
#define        IOI_audio_info          4
#define        IOI_rd_drv_bytes        5
#define        IOI_dev_status          6
#define        IOI_ret_sectsize        7
#define        IOI_ret_volsize         8
#define        IOI_media_changed       9
#define        IOI_audio_diskinfo      10
#define        IOI_audio_trackinfo     11
#define        IOI_audio_qchaninfo     12
#define        IOI_audio_subinfo       13
#define        IOI_upc_code            14
#define        IOI_cmd_max             14

#define        IOO_eject_disc          0
#define        IOO_lock_door           1
#define        IOO_reset_drv           2
#define        IOO_set_audio_param     3
#define        IOO_wr_drv_bytes        4
#define        IOO_cmd_max             4

#define        error_drive_not_ready   21

typedef        unsigned char   uchar;
typedef        unsigned short  ushort;
typedef        unsigned int    uint;
typedef        unsigned long   ulong;

       /* Device driver request header */
typedef struct Request_Hdr {
       uchar           rqh_len;
       uchar           rqh_unit;
       uchar           rqh_cmd;
       ushort          rqh_status;
       uchar           rqh_rsvd[8];
       } Request_Hdr;

       /* Request header for INIT function */
typedef struct Init_Hdr {
       Request_Hdr     init_rqh;
       uchar           init_units;
       uchar   far     *init_endaddr;
       uchar   far     *init_bpbarr;
       uchar           init_devno;
       } Init_Hdr;

       /* Request header for IOCTL command */
typedef struct Ioctl_Hdr {
       Request_Hdr     ioctl_rqh;
       uchar           ioctl_media;
       uchar far       *ioctl_xfer;
       ushort          ioctl_nbytes;
       ushort          ioctl_sector;
       uchar far       *ioctl_volid;
       } Ioctl_Hdr;

       /* Request header for Read/Write command */
typedef struct ReadWriteL_Hdr {
       Request_Hdr     rwl_rqh;
       uchar           rwl_addrmd;
       uchar far       *rwl_xfer;
       ushort          rwl_nsects;
       ulong           rwl_sectno;
       uchar           rwl_mode;
       uchar           rwl_ilsize;
       uchar           rwl_ilskip;
       ushort          rwl_reqno;
       } ReadWriteL_Hdr;

       /* Request header for Play Audio command */
typedef struct PlayReq_Hdr {
       Request_Hdr     pl_rqh;
       uchar           pl_addrmd;
       ulong           pl_start;
       ulong           pl_num;
       } PlayReq_Hdr;

       /* Record for audio_diskinfo IOCTL call */
typedef struct DiskInfo_Rec {
       uchar   cmd_code;
       uchar   lo_tno;
       uchar   hi_tno;
       ulong   lead_out;
       } DiskInfo_Rec;

       /* Record for UPC code IOCTL call */
typedef struct UPCCode_Rec {
       uchar   cmd_code;
       uchar   ctrl_adr;
       uchar   upc[7];
       uchar   zero;
       uchar   aframe;
       } UPCCode_Rec;

       /* Record for audio_trackinfo IOCTL call */
typedef struct TnoInfo_Rec {
       uchar   cmd_code;
       uchar   tno;
       ulong   start_addr;
       uchar   ctrl;
       } TnoInfo_Rec;

       /* Record for audio_qchaninfo IOCTL call */
typedef struct QchanInfo_Rec {
       uchar   cmd_code;
       uchar   ctrl;
       uchar   tno;
       uchar   x;
       uchar   min;
       uchar   sec;
       uchar   frame;
       uchar   zero;
       uchar   pmin;
       uchar   psec;
       uchar   pframe;
       } QchanInfo_Rec;

       /* Format for CDROM device driver header at CS:0 of device driver
       ** Slightly extended from standard MSDOS character device driver
       */
typedef struct Dev_Hdr {
       struct Dev_Hdr far *sdevnext;   /* Pointer to next dev header */
       ushort    sdevatt;              /* Attributes of the device   */
       void      (*sdevstrat)();       /* Strategy entry point       */
       void      (*sdevint)();         /* Interrupt entry point      */
       uchar     sdevname[8];          /* Name of device (only first byte used for block) */
       ushort    sdevrsvd;             /* Reserved word              */
       uchar     sdevlet;              /* Drive letter of first unit */
       uchar     sdevunits;            /* Number of units handled    */
       } Dev_Hdr;

       /* Record format for information returned with Get CDROM drive 
       ** letter device list function request
       */
typedef struct Dev_List {
       uchar           sub_unit;
       Dev_Hdr far     *dev_addr;
       } Dev_List;

       /* We use 100 tracks as there are 1-99 tracks on disk and we
       ** use the 100th one to store the location of the lead-out
       ** track.
       */
TnoInfo_Rec    TnoInfo[99 + 1];        /* Table of info for all tracks */
DiskInfo_Rec   DiskInfo;               /* Disk information for CD      */
UPCCode_Rec    UPCCode;                /* UPC code for CD              */
QchanInfo_Rec  QInfo;                  /* Space for returned qchan info*/
Dev_List       Dev_Tbl[26];            /* Table for all device drivers */
ushort         Num_Drives;             /* Number of CDROM drives       */
ushort         First_DrvLetter;        /* First letter used by CDROM's */
PlayReq_Hdr    Play_Rec;               /* Record for Play requests     */
Ioctl_Hdr      Ioctl_Rec;              /* Record for IOCTL requests    */

       /* External masm routine that sets up request to be passed to
       ** the device driver.
       */
extern void    send_req(ReadWriteL_Hdr far *, Dev_Hdr far *);



/* red2hsg() -
**
** DESCRIPTION
**     Converts a binary red book address to high sierra addressing
**     The msb of the red book is always zero, the next less significant
**     byte is the minute (0-59+), then second (0-59) and lsb is the
**     frame (0-75). The conversion is
**             hsg = min * 60 * 75 + sec * 75 + frame;
*/
ulong red2hsg(l)
ulong  l;
       {
       return((ulong) (HIWORD(l) & 0xff) * 60 * 75 +
               (ulong) (LOWORD(l) >> 8) * 75 +
               (ulong) (LOWORD(l) & 0xff));
       }


/* play() -
**
** DESCRIPTION
**     Sends the request to play num frames at address start on
**     drive drv. Mode determines whether the starting address is to
**     be interpreted as high sierra or red book addressing.
**     If num == 0, then instead of sending a PLAY-AUDIO command,
**     we issue a STOP-AUDIO command.
*/
uint play(drv, start, num, mode)
Dev_List       *drv;
ulong          start;
ulong          num;
uchar          mode;
       {
       register PlayReq_Hdr    *req = &Play_Rec;
       
       req->pl_rqh.rqh_len     = sizeof(PlayReq_Hdr);
       req->pl_rqh.rqh_unit    = drv->sub_unit;
       req->pl_rqh.rqh_cmd     = (uchar) (num ? DEVPLAY : DEVSTOP);
       req->pl_rqh.rqh_status  = 0;

       req->pl_addrmd  = mode;
       req->pl_start   = start;
       req->pl_num     = num;

       send_req((ReadWriteL_Hdr far *) req, drv->dev_addr);
       if (req->pl_rqh.rqh_status & 0x8000) {
               printf("        Error on play - status = 0x%r\n", req->pl_rqh.rqh_status, 0);
               return(error_drive_not_ready);
               }

       return(0);
       }

/* ioctl() -
**
** DESCRIPTION
**     Sends an IOCTL request to the device driver in drv. The
**     ioctl command is cmd and the ioctl cmd length is cmdlen.
**     The buffer for the command is pointed to by xbuf.
*/
uint ioctl(drv, xbuf, cmd, cmdlen)
Dev_List       *drv;
uchar          *xbuf;
uchar          cmd;
uchar          cmdlen;
       {
       register Ioctl_Hdr      *io = &Ioctl_Rec;

       io->ioctl_rqh.rqh_len   = sizeof(Ioctl_Hdr);
       io->ioctl_rqh.rqh_unit  = drv->sub_unit;
       io->ioctl_rqh.rqh_cmd   = DEVRDIOCTL;
       io->ioctl_rqh.rqh_status = 0;

       io->ioctl_media         = 0;
       io->ioctl_xfer          = (char far *) xbuf;
       *xbuf                   = cmd;
       io->ioctl_nbytes        = cmdlen;
       io->ioctl_sector        = 0;
       io->ioctl_volid         = 0;

       send_req((ReadWriteL_Hdr far *) io, drv->dev_addr);
       if (io->ioctl_rqh.rqh_status & 0x8000) {
               printf("        Error on play - status = 0x%r\n", io->ioctl_rqh.rqh_status);
               return(error_drive_not_ready);
               }
       return(0);
       }

/* read_toc() -
**
** DESCRIPTION
**     Reads the disk information from the TOC in the qchannel
**     to find the first and last track numbers and builds a
**     table (TnoInfo) of the starting address for each track by 
**     asking the device driver for the info for each track.
*/
void read_toc(drv)
Dev_List       *drv;
       {
       TnoInfo_Rec     *t;
       QchanInfo_Rec   *q = &QInfo;
       uchar           *s;
       uint            i;

       if (ioctl(drv, &DiskInfo, IOI_audio_diskinfo, sizeof(DiskInfo_Rec)))
               printf("        Failed to read TOC\n");

               /* The entry after the last track has as it's
               ** starting address the beginning of the lead-out
               ** track which is the end of the audio on the disc.
               ** This is why we have 99+1 records in TnoInfo
               */
       TnoInfo[DiskInfo.hi_tno + 1].start_addr = DiskInfo.lead_out;

       t = &TnoInfo[DiskInfo.lo_tno];
       for (i = DiskInfo.lo_tno; i <= DiskInfo.hi_tno; i++) {
               t->tno = (uchar) i;
               ioctl(drv, t, IOI_audio_trackinfo, sizeof(TnoInfo_Rec));
               t++;
               }
       }

/* play_tracks() -
**
** DESCRIPTION
**     If WHOLE is defined, then we play the entire disc.
**     Otherwise we play about 10 seconds (the amount of time
**     to do 200 qchannel queries) from each track on the
**     disk
*/
void play_tracks(drv)
Dev_List       *drv;
       {
       TnoInfo_Rec     *t;
       QchanInfo_Rec   *q = &QInfo;
       uint            i;
       uint            j;
       uint            k;
       ulong           num;
       uchar           *s;

       t = &TnoInfo[DiskInfo.lo_tno];

       num = red2hsg(DiskInfo.lead_out) - red2hsg(t->start_addr);
       play(drv, t->start_addr, num, ADDR_RED);
       }

/* find_drivers() -
**
**     Using INT 2Fh with AH=15h (the MSCDEX function request interface)
**     we can ask MSCDEX for the number and location of all CDROM
**     device drivers on the system.
**
**     Unfortunately at present, if MSCDEX is not present, the carry
**     flag is not set when an INT 2Fh is issued with AH=15h...I'll 
**     figure out what's going on and figure out a good way to tell
**     if MSCDEX is present or not.
*/
void find_drivers()
       {
       union REGS      inregs;
       union REGS      outregs;
       struct SREGS    segregs;
       uchar far       *d = (uchar far *) Dev_Tbl;
       Dev_Hdr far     *sdev;
       uint            i;

       inregs.x.ax = 0x1500;   /* Get Number of CDROM drive letters    */
       inregs.x.bx = 0;        /* Init to zero                         */
       int86(0x2f, &inregs, &outregs);

               /* If number of drives returned is still 0, then MSCDEX
               ** is not installed.
               */
       if (outregs.x.bx == 0) {
               printf("MSCDEX not installed\n");
               exit(1);
               }

       Num_Drives      = outregs.x.bx;
       First_DrvLetter = outregs.x.cx;

       inregs.x.ax = 0x1501;   /* Get CDROM drive letter device list   */
       inregs.x.bx  = LOWORD(d);
       segregs.es = HIWORD(d);
       int86x(0x2f, &inregs, &outregs, &segregs);

               /* Check if carry set           */
       if (outregs.x.cflag) {
               printf("MSCDEX not present\n");
               exit(1);
               }

       for (i = 0; i < Num_Drives; i++) {
               sdev = Dev_Tbl[i].dev_addr; 
               }

       }

void main()
       {
       find_drivers();
       read_toc(&Dev_Tbl[0]);          
       play_tracks(&Dev_Tbl[0]);
       }












