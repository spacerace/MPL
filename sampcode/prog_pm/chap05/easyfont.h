/*---------------------------------------
   EASYFONT.H header file for EASYFONT.C
  ---------------------------------------*/

BOOL EzfQueryFonts    (HPS hps) ;
LONG EzfCreateLogFont (HPS hps, LONG lcid, USHORT idFace, USHORT idSize,
                                           USHORT fsSelection) ;

#define FONTFACE_SYSTEM  0
#define FONTFACE_COUR    1
#define FONTFACE_HELV    2
#define FONTFACE_TIMES   3

#define FONTSIZE_8       0
#define FONTSIZE_10      1
#define FONTSIZE_12      2
#define FONTSIZE_14      3
#define FONTSIZE_18      4
#define FONTSIZE_24      5
