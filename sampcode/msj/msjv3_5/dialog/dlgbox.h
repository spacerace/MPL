/* DLGBOX.H - Header File */

#define IDM_MODAL         100
#define IDM_MODELESS      110
#define IDM_ONE           120

#define IDDEDIT        3025
#define IDCHECK        3026

/* Control class codes */

#define BUTTONCLASS    0x80 /* 128 */
#define EDITCLASS      0x81 /* 129 */
#define STATICCLASS    0x82 /* 130 */
#define LISTBOXCLASS   0x83 /* 131 */
#define SCROLLBARCLASS 0x84 /* 132 */

typedef struct DialogHeader{
   long  dtStyle;
   BYTE  dtItemCount;
   int   dtX;
   int   dtY;
   int   dtCX;
   int   dtCY;
} DLGHDR;

/* The following 3 header items are variable strings so not  part of DLGHDR */

/*   char dtResourceName[]; */
/*   char dtClassName[];    */
/*   char dtCaptionText[];  */

typedef struct DialogItem {
   int   dtilX;
   int   dtilY;
   int   dtilCX;
   int   dtilCY;
   int   dtilID;
   long  dtilStyle;
   BYTE  dtilControlClass;
} DLGITEM;

/* The following control items are variable so not part of DLGITEM */

/*   char dtilText[]; */
/*   BYTE dtilInfo; */

#define  DI0   0L
#define  DI1   1L
#define  DI2   2L
#define  DI3   3L
#define  DI4   4L
#define  DI5   5L
#define  DI6   6L
#define  DI7   7L
#define  DI8   8L
#define  DI9   9L
#define  DI10  10L
#define  DI11  11L
#define  DI12  12L
#define  DI13  13L
#define  DI14  14L
#define  DI15  15L
#define  DI16  16L
#define  DI17  17L
#define  DI18  18L
#define  DI19  19L
#define  DI19  19L
#define  DI20  20L
#define  DI21  21L

