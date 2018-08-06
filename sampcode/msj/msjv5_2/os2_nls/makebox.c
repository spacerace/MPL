/********************************* MAKEBOX ************************\
*
*  VIO DBCS SAMPLE program
*
*  This program puts up panel containing a message into which
*  the first 3 command line arguments have been substituted.
*
*  This program will run on any OS/2 1.2 version.
*  However, most of the DBCS machinerie in it lies unused if run
*  in a non-DBCS code page or on non-DBCS hardware.
*
*  It is therefore typical of an 'international application' which
*  also contains that bit of extra code to allow it to run
*  everywhere.
*
*  This program works so it can be run as a second process in a
*  screen group, i.e. it adapts to the VIO mode, VIO code page etc
*  that were set up by the first process.  Alternatively this code
*  would work well in a DLL.
*
\******************************************************************/

#define INCL_VIO
#define INCL_DOSNLS
#define INCL_DOSMISC
#define INCL_DOSPROCESS

#include <os2.h>
#include <ctype.h>
#include <stdio.h>
#include "makebox.h"

BYTE   fbLead[256];          // lead byte table, initially all 0
                             // (i.e. FALSE or not a lead byte)

#define fIsDbcsLead( x )  (fbLead[(x)])

#define CBMAXMSG  200       // upper bound for message text

/**  MAIN:
 **
 **  Construct a message panel and display
 **  the arguments in a message text
 **/
main( argc, argv )
SHORT   argc;
CHAR  ** argv;
{
 USHORT      ib, ich, iLo, iHi;

 USHORT      usCodePage;     // code-page identifier
 USHORT      usReserved=0;   // must be zero

 USHORT      cbBuf;          // length of buffer
 COUNTRYCODE ctryc;          // structure for country code
 CHAR        abDbcsRange[CBMAXDBCS];  // buffer for DBCS info

 CHAR        achMsgBuf[CBMAXMSG];  // return buffer for Text
 USHORT      cbMsg;                // # of message bytes returned
 PCHAR       apchVTable[10];       // pointer to table of pointers
                                   // pointers to strings
 SHORT       iTmp;

 USHORT      xLeft,                // Message box coordinates
             xRight,
             yTop,
             yBottom;

 VIOSCROLL   vioscroll;            // VioGetState scroll info block

 VIOMODEINFO viomi;                // Video mode information


 //   First, determine where to locate the message box.
 //   Get the current Vio mode to find the number of lines on the
 //   screen.

 viomi.cb = sizeof(VIOMODEINFO);
 VioGetMode(&viomi, 0);

 //   note: bottom screen line is used for keybd status in DCBS
 //   countries so we use VioGetState to return the size of this
 //   (nonscrollable) area

 vioscroll.cb = sizeof(vioscroll);
 vioscroll.type = VS_GETSCROLL;

 VioGetState((PVIOSCROLL) &vioscroll, VIOHANDLE);

 // Set Message box coordinates to stay inside screen area

 xLeft   = 10,
 xRight  = viomi.col - 9,
 yTop    = 5,
 yBottom = viomi.row - vioscroll.cnscrl - 5;


 //  Get Current VIO Code Page

 VioGetCp(0,                         // must be zero
         &usCodePage,                // code-page identifier
         VIOHANDLE);                 // video handle

 //   Get Lead Byte range (if any) for current VIO code page
 //   (for SBCS code page abDbcsRange is all 0)

 ctryc.country = 0;
 ctryc.codepage = usCodePage;
 DosGetDBCSEv(cbBuf, (PCOUNTRYCODE) &ctryc, (PCHAR) abDbcsRange);

 //   Construct lead byte table for quick parsing of DBCS strings

 for (ib=0; ib < cbBuf && abDbcsRange[ib]; ib++){
         if( ib % 1 ){
                 for( ich = iLo; ich <= abDbcsRange[ib]; ich++ )
                         fbLead[ich] = 1;
         } else
                 iLo = abDbcsRange[ib];
 }

 //   Synch Process and VIO code page, so  DosGetMessage retrieves
 //   characters that VIO can display.

 DosSetProcCp(usCodePage, usReserved);

 // Save screen area to be obscured by message box.

 SaveArea(yTop, xLeft, yBottom, xRight, TRUE);

 //   Retrieve frame characters

 DosGetMessage(
    (PCHAR FAR *) NULL,      // pointer to table of ptrs to strings
    0,                       // number of pointers in table
    (PBYTE) achMsgBuf,       // buffer receiving message
    CBMAXMSG,                // size of message buffer
    MSG_APPL_SINGLEFRAMECHAR,// message number to retrieve
    "oso001.msg",            // name of file containing message
    (PUSHORT) &cbMsg);       // number of bytes in returned message


 //   Parse the frame characters message and draw a box

 ParseFrameMsg( achMsgBuf, cbMsg);
 MakeBox(yTop, xLeft, yBottom, xRight);

 // Retrieve message text; merge w/ first few arguments from argv

 argc = (argc > 4 ? 4 : argc );
 for ( iTmp = 0; iTmp < argc; iTmp++) {
         apchVTable[iTmp] = (PCHAR) argv[iTmp];
 }

 DosGetMessage(
   apchVTable,            // pointer to table of pointers to strings
   argc,                  // number of pointers in table
   (PCHAR) achMsgBuf,     // buffer receiving message
   CBMAXMSG,              // size of message buffer
   0,                     // message number to retrieve
   "makebox.msg",         // name of file containing message
   (PUSHORT) &cbMsg);     // number of bytes in returned message

 //   Set Cursor to beginning of message text

 VioSetCurPos(yTop + 2,               // cursor row
              xLeft + 2,              // cursor column
              VIOHANDLE);             // video handle

 //   Output message text

 VioWrtTTY(achMsgBuf, cbMsg, VIOHANDLE);


 /**
  **  User interaction here
       .
       .
       .
  **/

 DosSleep( 2000L );

 //   Restore screen from saved area

 SaveArea(yTop, xLeft, yBottom, xRight, FALSE);

 return 0;
}

// The boxes defined here have black on red borders and a white
// interior.  The attributes are ULONG so they work correctly
//      with the world-wide LVB format's 3 byte attributes.
//

USHORT  ausBoxChar[3][3];       // Skeleton for a box frame
static ULONG aulAttr[][3] =     // Corresponding char attribute vals

                                {  {0x40, 0x40, 0x40},
         {0x40, 0x70, 0x40},
         {0x40, 0x40, 0x40}
        };


/**   PARSEFRAMEMSG:
 **
 **   Collect the frame characters from the message and place them
 **   into a skeleton 3x3 array for the box.
 **/

VOID ParseFrameMsg( pchMsg, cbMsg )
PCH pchMsg;
USHORT cbMsg;
{
    sscanf( (char *)pchMsg, "%x %x %x %x %x %x %x %x",
            &ausBoxChar[0][0],
            &ausBoxChar[0][2],
            &ausBoxChar[2][0],
            &ausBoxChar[2][2],
            &ausBoxChar[1][0],
            &ausBoxChar[1][2],
            &ausBoxChar[2][1],
            &ausBoxChar[0][1]
            );
    ausBoxChar[1][1] = 0x20;
}


/**  MAKEBOX:
 **
 **  Expand Box skeleton to draw box of the desired size.  Uses VIO
 **  calls exclusively in order to be portable to different H/W.
 **/

VOID MakeBox(yTop, xLeft, yBottom, xRight)
USHORT xLeft,                       // Box coordinates
       xRight,
       yTop,                        // not inclusive
       yBottom;
{

    static int iTimes[] = {1, 1, 1};
    int i1, i2, xTemp;
    USHORT usChar;

    iTimes[1] = xRight - xLeft;
    xTemp = xLeft;

    // The nested loops below repeatedly draw the center row and
    // center column  of the box skeleton, thereby expanding the
    // box to the desired size.

    for (i1 = 0; i1 < 3; i1++)
 do  {
     for (i2 = 0; i2 < 3; i2++)  {

                //  Write Frame Character

                usChar = ausBoxChar[i1][i2];

   VioWrtNChar((PCH)&usChar, iTimes[i2],yTop, xLeft, VIOHANDLE);

                //  Write corresponding attribute
                //  (need to write twice for DBCS frame chars)

   VioWrtNAttr((PBYTE)&aulAttr[i1][i2], iTimes[i2],
                        yTop, xLeft, VIOHANDLE);
                if( fIsDbcsLead( usChar & 0xFF)){
           VioWrtNAttr((PBYTE)&aulAttr[i1][i2], iTimes[i2],
     yTop, xLeft, VIOHANDLE);
                }
   xLeft += iTimes[i2];
     }
     xLeft = xTemp;
     yTop++;
 }  while ((i1 == 1) && (yTop < yBottom));

} /* end MakeBox */


/** SAVEAREA:
 **
 ** This function saves and restores the area of the screen that
 ** will be obscured by our message panel.  Care is taken that any
 ** DBCS chars which might be straddling the edges of the rectangle
 ** are saved and restored correctly.
 **
 ** This problem may exist if the rectangle to be saved doesn't
 ** extend to the screen edge.  It is handled by saving an extra
 ** cell on each side. The saved buffer will contain the full DBCS
 ** character for for any DBCS charactrers that were straddling the
 ** initial rectangle.
 **
 ** Before restoring we need to know which of the extra cells
 ** contain such halves of DBCS characters necessary for completion.
 ** VioCheckCharType is used to check the cell in the video buffer.
 **/
VOID SaveArea(yTop, xLeft, yBottom, xRight, fSave)
USHORT xLeft,                // area coordinates
       xRight,
       yTop,                 // not inclusive
       yBottom;
BOOL fSave;
{

    static SEL selArea;      // Selector for saved segment
    static PBYTE pbArea;     // Pointer to saved screen area

    PBYTE pbLine;            // Pointer to line being drawn
    SHORT cbmax, cbCur, xCur, cbCell, cbArea;
    VIOMODEINFO viomi;       // Video mode information

    USHORT usType;           // Cell type

    //Get the current Vio mode to find out whether we have 1 or 3
    //attribute bytes per cell. Set the size of the saved area
    //accordingly;

    viomi.cb = sizeof(VIOMODEINFO);
    VioGetMode(&viomi, 0);
    cbCell = viomi.attrib+1;

    //   Calculate maximum number of bytes per line to be saved

    cbmax = (xRight - xLeft + 3) * cbCell;

    //   Allocate buffer to hold saved area

    if( fSave ){
        cbArea = (yBottom - yTop + 1) * cbmax;

        DosAllocSeg(cbArea,          //bytes to allocate
                &selArea,            //address of selector
                SEG_NONSHARED);      //sharing flag
    }
    pbArea = MAKEP(selArea, 0);      //convert selector to pointer

    do  {
        //  Number of bytes per line to be saved and
        //  starting offset for this line

        cbCur = cbmax;
        pbLine = pbArea + (yBottom - yTop) * cbmax;

        //  Extend area to be saved by 1 col Left & Right
        //  (unless we are already at the edge of the screen)

        if(xLeft )
                 xCur = xLeft-1;
        else
                 xCur=xLeft, cbCur-= cbCell;

        if ( xRight == viomi.col )
                cbCur-= cbCell;

        if( fSave ){

             //  Save one line

      VioReadCellStr(pbLine,(PUSHORT) &cbCur,yTop,xCur,VIOHANDLE);

        } else {

             //  Inspect the byte left of the obscured area:
             //  If it's a trail byte, our saved string
             //  begins with this trail byte which is illegal.
             //  Advance our string pointer and start column by one.

             if(xLeft > xCur){
                 VioCheckCharType( (PUSHORT) &usType, yTop, xCur,
                                   VIOHANDLE);
                 if( usType == VCC_DBCS2NDHALF ) {

                    //  skip trail byte on write

                    xCur = xLeft;
                    cbCur -= cbCell;
                    pbLine += cbCell;
                 }
             }

             //  As above, we cannot end a write on a lead byte.

             if ( xRight < viomi.col ){
                 VioCheckCharType( (PUSHORT) &usType, yTop, xRight+1,
                                   VIOHANDLE);
                 if( usType == VCC_DBCS1STHALF )

                       //   don't end write on lead byte

                       cbCur-= cbCell;
             }

             //  Restore one line

      VioWrtCellStr(pbLine, cbCur, yTop, xCur, VIOHANDLE);
 }
    }  while (++yTop <= yBottom);

    //   Free memory after restore

    if( !fSave ){
        DosFreeSeg( selArea );
    }

}  /* end SaveArea */
