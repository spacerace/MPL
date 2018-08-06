/*------------------------
   VECTFONT.H header file
  ------------------------*/

#define ID_RESOURCE      1

#define IDM_NOTHING      0
#define IDM_24POINT      1
#define IDM_STRETCH      2
#define IDM_MIRROR       3
#define IDM_CHARANGLE    4
#define IDM_ROTATE       5
#define IDM_CHARSHEAR    6
#define IDM_SHADOW       7
#define IDM_HOLLOW       8
#define IDM_DROPSHADOW   9
#define IDM_BLOCK        10
#define IDM_NEON         11
#define IDM_FADE         12
#define IDM_SPOKES       13
#define IDM_WAVY         14
#define IDM_MODSPOKES    15

#define LCID_MYFONT 1L
#define ID_PATH     1L
#define PI          3.14159

LONG CreateVectorFont (HPS hps, LONG lcid, CHAR *szFacename) ;   // VF00
BOOL ScaleVectorFont (HPS hps, SHORT xPointSize, SHORT yPointSize) ;
BOOL ScaleFontToBox (HPS hps, LONG cbText, CHAR *szText, LONG cxBox,
                                                         LONG cyBox) ;
VOID QueryStartPointInTextBox (HPS hps, LONG cbText, CHAR *szText,
                                        POINTL *pptl) ;
VOID ColorClient (HPS hps, LONG cxClient, LONG cyClient, LONG lColor) ;

VOID Display_24Point (HPS hps, LONG cxClient, LONG cyClient) ;   // VF01
VOID Display_Stretch (HPS hps, LONG cxClient, LONG cyClient) ;   // VF02
VOID Display_Mirror (HPS hps, LONG cxClient, LONG cyClient) ;    // VF03
VOID Display_CharAngle (HPS hps, LONG cxClient, LONG cyClient) ; // VF04
VOID Display_Rotate (HPS hps, LONG cxClient, LONG cyClient) ;    // VF05
VOID Display_CharShear (HPS hps, LONG cxClient, LONG cyClient) ; // VF06
VOID Display_Shadow (HPS hps, LONG cxClient, LONG cyClient) ;    // VF07
VOID Display_Hollow (HPS hps, LONG cxClient, LONG cyClient) ;    // VF08
VOID Display_DropShadow (HPS hps, LONG cxClient, LONG cyClient) ;// VF09
VOID Display_Block (HPS hps, LONG cxClient, LONG cyClient) ;     // VF10
VOID Display_Neon (HPS hps, LONG cxClient, LONG cyClient) ;      // VF11
VOID Display_Fade (HPS hps, LONG cxClient, LONG cyClient) ;      // VF12
VOID Display_Spokes (HPS hps, LONG cxClient, LONG cyClient) ;    // VF13
VOID Display_Wavy (HPS hps, LONG cxClient, LONG cyClient) ;      // VF14
VOID Display_ModSpokes (HPS hps, LONG cxClient, LONG cyClient) ; // VF15
