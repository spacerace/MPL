/* ==================================================================
	CVIDEO.C - C video modules.
================================================================== */

#include <dos.h>

struct VideoInfo {
   unsigned char mode;
   unsigned char rows;
   unsigned char columns;
   unsigned char ColorFlag;
   unsigned char SyncFlag;
   unsigned int BufferStart;
   unsigned int SegAddr;
};

void GetVideoParms(struct VideoInfo *);
void ClrScr(char, struct VideoInfo *);
void ClrRegion(char, char, char, char, char);
void TextBox(char, char, char, char, char, struct VideoInfo *);
void SaveRegion(char, char, char, char, int *, struct VideoInfo *);
void RestRegion(char, char, char, char, int *, struct VideoInfo *);

extern int GetCharAttr(char, char, struct VideoInfo *);
extern void DispCharAttr(char, char, char, char, struct VideoInfo *);

struct VideoInfo video;
union REGS inregs, outregs;

/* ==================================================================
   GetVideoParms() fills a video structure of type 
   VideoInfo with data.
=================================================================== */
void GetVideoParms(struct VideoInfo *sptr)
{
   char far *CrtMode    = (char far *) 0x00400049;
   char far *CrtCols    = (char far *) 0x0040004A;
   int  far *CrtStart   = (int far *) 0x0040004E;
   int  far *CrtAddr    = (int far *) 0x00400063;
   char far *CrtRows    = (char far *) 0x00400084;
   char far *CrtEgaInfo = (char far *) 0x00400087;

   /* --- Determine whether video writes must be synchronized --- */
   inregs.h.ah = 0x12;
   inregs.h.bl = 0x10;
   int86(0x10, &inregs, &outregs);
   if (outregs.h.bl == 0x10) {
       if (*CrtAddr == 0x3D4)
           sptr->SyncFlag = 1;
       else
           sptr->SyncFlag = 0;
       sptr->rows = 25;
   }
   else {
       if ((*CrtEgaInfo & 0x08) && (*CrtAddr == 0x3D4))
           sptr->SyncFlag = 1;
       else
           sptr->SyncFlag = 0;
       sptr->rows = *CrtRows + 1;
   }

   /* ---- Determine whether video is color or monochrome ---- */
   if (*CrtAddr == 0x3D4) {
       sptr->ColorFlag = 1;
       sptr->SegAddr = 0xB800;
   }
   else {
       sptr->ColorFlag = 0;
       sptr->SegAddr = 0xB000;
   }

   /* ----- Copy remaining parameters from BIOS ----- */
   sptr->mode = *CrtMode;
   sptr->columns = *CrtCols;
   sptr->BufferStart = *CrtStart;
}

/* ==================================================================
   ClrScr() clears the entire viewing area.
================================================================== */
void ClrScr(char VideoAttr, struct VideoInfo *sptr)
{
   inregs.x.ax = 0x0600;
   inregs.h.bh = VideoAttr;
   inregs.x.cx = 0x0000;
   inregs.h.dh = sptr->rows - 1;
   inregs.h.dl = sptr->columns - 1;
   int86(0x10, &inregs, &outregs);
}

/* ==================================================================
   ClrRegion() clears the specified screen region.
================================================================== */
void ClrRegion(char row1, char col1, char row2, char col2, 
               char VideoAttr)
{
   inregs.x.ax = 0x0600;
   inregs.h.bh = VideoAttr;
   inregs.h.ch = row1;
   inregs.h.cl = col1;
   inregs.h.dh = row2;
   inregs.h.dl = col2;
   int86(0x10, &inregs, &outregs);
}

/* ==================================================================
   TextBox() draws a box around the specified region.
================================================================== */
void TextBox(char row1, char col1, char row2, char col2, 
             char VideoAttr, struct VideoInfo *sptr)
{
   char i;

   DispCharAttr(0xDA, row1, col1, VideoAttr, sptr);
   for (i=col1+1; i<col2; i++)
       DispCharAttr(0xC4, row1, i, VideoAttr, sptr);
   DispCharAttr(0xBF, row1, col2, VideoAttr, sptr);
   for (i=row1+1; i<row2; i++)
       DispCharAttr(0xB3, i, col2, VideoAttr, sptr);
   DispCharAttr(0xD9, row2, col2, VideoAttr, sptr);
   for (i=col2-1; i>col1; i--)
       DispCharAttr(0xC4, row2, i, VideoAttr, sptr);
   DispCharAttr(0xC0, row2, col1, VideoAttr, sptr);
   for (i=row2-1; i>row1; i--)
       DispCharAttr(0xB3, i, col1, VideoAttr, sptr);
}

/* ==================================================================
   SaveRegion() saves the contents of a specified screen region.
================================================================== */
void SaveRegion(char row1, char col1, char row2, char col2,
                int *ScreenBuffer, struct VideoInfo *sptr)
{
   char i, j;

   for (i=row1; i<=row2; i++)
       for (j=col1; j<=col2; j++)
           *ScreenBuffer++ = GetCharAttr(i, j, sptr);
}

/* ==================================================================
   RestRegion() restores the contents of a specified screen region.
================================================================== */
void RestRegion(char row1, char col1, char row2, char col2,
                int *ScreenBuffer, struct VideoInfo *sptr)
{
   char i, j;

   for (i=row1; i<=row2; i++)
       for (j=col1; j<=col2; j++)
           DispCharAttr((char) (*ScreenBuffer++ & 0x00FF), i, j,
                        (char) (*ScreenBuffer >> 8), sptr);
}
