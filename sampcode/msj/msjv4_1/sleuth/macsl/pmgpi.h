/***************************************************************************\
*
* Module Name: PMGPI.H
*
* OS/2 Presentation Manager GPI constants, types and function declarations
*
* Copyright (c) 1987  IBM Corporation
* Copyright (c) 1987  Microsoft Corporation
*
* =============================================================================
*
* The following symbols are used in this file for conditional sections.
*
*   INCL_GPI                Include all of the GPI
*   INCL_GPICONTROL         Basic PS control
*   INCL_GPICORRELATION     Picking, Boundary and Correlation
*   INCL_GPISEGMENTS        Segment Control and Drawing
*   INCL_GPISEGEDITING      Segment Editing via Elements
*   INCL_GPITRANSFORMS      Transform and Transform Conversion
*   INCL_GPIPATHS           Paths and Clipping with Paths
*   INCL_GPILOGCOLORTABLE   Logical Color Tables
*   INCL_GPIPRIMITIVES      Drawing Primitives and Primitive Attributes
*   INCL_GPILCIDS           Phyical and Logical Fonts with Lcids
*   INCL_GPIBITMAPS         Bitmaps and Pel Operations
*   INCL_GPIREGIONS         Regions and Clipping with Regions
*   INCL_GPIMETAFILES       Metafiles
*   INCL_GPIERRORS          defined if INCL_ERRORS defined
*
* There is a symbol used in this file called INCL_DDIDEFS. This is used to
* include only the definitions for the DDI. The programmer using the GPI
* can ignore this symbol
*
* There is a symbol used in this file called INCL_SAADEFS. This is used to
* include only the definitions for the SAA. The programmer using the GPI
* can ignore this symbol
*
\***************************************************************************/
 
#ifdef INCL_GPI /* include whole of the GPI */
    #define INCL_GPICONTROL
    #define INCL_GPICORRELATION
    #define INCL_GPISEGMENTS
    #define INCL_GPISEGEDITING
    #define INCL_GPITRANSFORMS
    #define INCL_GPIPATHS
    #define INCL_GPILOGCOLORTABLE
    #define INCL_GPIPRIMITIVES
    #define INCL_GPILCIDS
    #define INCL_GPIBITMAPS
    #define INCL_GPIREGIONS
    #define INCL_GPIMETAFILES
#endif /* INCL_GPI */
 
#ifdef INCL_ERRORS /* if errors are required then allow GPI errors */
    #define INCL_GPIERRORS
#endif /* INCL_ERRORS */
 
#ifdef INCL_DDIDEFS /* if only DDI required then enable DDI part of GPI */
    #define INCL_GPITRANSFORMS
    #define INCL_GPIPATHS
    #define INCL_GPILOGCOLORTABLE
    #define INCL_GPIPRIMITIVES
    #define INCL_GPILCIDS
    #define INCL_GPIBITMAPS
    #define INCL_GPIREGIONS
    #define INCL_GPIERRORS
#endif /* INCL_DDIDEFS */
 
#ifdef INCL_SAADEFS /* if only SAA required then enable SAA part of GPI */
    #define INCL_GPICONTROL
    #define INCL_GPICORRELATION
    #define INCL_GPISEGMENTS
    #define INCL_GPITRANSFORMS
    #define INCL_GPIPRIMITIVES
    #define INCL_GPILCIDS
    #define INCL_GPIMETAFILES
    #define INCL_GPIERRORS
#endif /* INCL_SAADEFS */
 
/* General GPI return values */
#define GPI_ERROR     0L
#define GPI_OK        1L
#define GPI_ALTERROR  (-1L)
 
/* fixed point number - implicit binary point between 2 and 3 hex digits */
typedef  LONG FIXED;     /* fx */
typedef  FIXED FAR *PFIXED;
 
/* make FIXED number from SHORT integer part and USHORT fractional part */
#define MAKEFIXED(intpart,fractpart) MAKELONG(fractpart,intpart)
/* extract fractional part from a fixed quantity */
#define FIXEDFRAC(fx)                (LOUSHORT(fx))
/* extract integer part from a fixed quantity */
#define FIXEDINT(fx)                 ((SHORT)HIUSHORT(fx))
 
/* structure for size parameters e.g. for GpiCreatePS */
typedef struct _SIZEL {         /* sizl */
    LONG cx;
    LONG cy;
} SIZEL;
typedef SIZEL FAR *PSIZEL;
 
#ifndef INCL_SAADEFS
 
/* return code on GpiQueryLogColorTable and GpiQueryPel */
#define CLR_NOINDEX  (-254L)
 
#endif /* no INCL_SAADEFS */
 
#ifdef INCL_GPICONTROL
 
/* units for GpiCreatePS and others */
#define PU_ARBITRARY    0x0004L
#define PU_PELS         0x0008L
#define PU_LOMETRIC     0x000CL
#define PU_HIMETRIC     0x0010L
#define PU_LOENGLISH    0x0014L
#define PU_HIENGLISH    0x0018L
#define PU_TWIPS        0x001CL
 
/* format for GpiCreatePS */
#define GPIF_DEFAULT    0L
#ifndef INCL_SAADEFS
#define GPIF_SHORT      0x0100L
#define GPIF_LONG       0x0200L
#endif /* no INCL_SAADEFS */
 
/* PS type for GpiCreatePS */
#define GPIT_NORMAL     0L
#ifndef INCL_SAADEFS
#define GPIT_MICRO      0x1000L
#endif /* no INCL_SAADEFS */
 
/* implicit associate flag for GpiCreatePS */
#define GPIA_NOASSOC    0L
#define GPIA_ASSOC      0x4000L
 
#ifndef INCL_SAADEFS
/* return error for GpiQueryDevice */
#define HDC_ERROR        ((HDC)-1L)
#endif /* no INCL_SAADEFS */
 
/*** common GPICONTROL functions */
HPS   APIENTRY GpiCreatePS( HAB, HDC, PSIZEL, ULONG );
BOOL  APIENTRY GpiDestroyPS( HPS );
BOOL  APIENTRY GpiAssociate( HPS, HDC );
 
#ifndef INCL_SAADEFS
BOOL APIENTRY GpiRestorePS( HPS, LONG );
LONG APIENTRY GpiSavePS( HPS );
BOOL APIENTRY GpiErase( HPS );
HDC  APIENTRY GpiQueryDevice( HPS );
#endif /* no INCL_SAADEFS */
 
#endif /* common GPICONTROL */
#ifdef INCL_GPICONTROL
 
/* options for GpiResetPS */
#define GRES_ATTRS      0x0001L
#define GRES_SEGMENTS   0x0002L
#define GRES_ALL        0x0004L
 
/* option masks for PS options used by GpiQueryPs */
#define PS_UNITS        0x00FCL
#define PS_FORMAT       0x0F00L
#define PS_TYPE         0x1000L
#define PS_MODE         0x2000L
#define PS_ASSOCIATE    0x4000L
 
#ifndef INCL_SAADEFS
 
/* error context returned by GpiErrorSegmentData */
#define GPIE_SEGMENT    0L
#define GPIE_ELEMENT    1L
#define GPIE_DATA       2L
 
/* control parameter for GpiSetDrawControl */
#define DCTL_ERASE            1L
#define DCTL_DISPLAY          2L
#define DCTL_BOUNDARY         3L
#define DCTL_DYNAMIC          4L
#define DCTL_CORRELATE        5L
 
/* constants for GpiSet/QueryDrawControl */
#define DCTL_ERROR -1L
#define DCTL_OFF    0L
#define DCTL_ON     1L
 
/* constants for GpiSet/QueryStopDraw */
#define SDW_ERROR -1L
#define SDW_OFF    0L
#define SDW_ON     1L
 
/* drawing for GpiSet/QueryDrawingMode */
#define DM_ERROR         0L
#define DM_DRAW          1L
#define DM_RETAIN        2L
#define DM_DRAWANDRETAIN 3L
 
#endif /* no INCL_SAADEFS */
 
/*** other GPICONTROL functions */
BOOL  APIENTRY GpiResetPS( HPS, ULONG );
BOOL  APIENTRY GpiSetPS( HPS, PSIZEL, ULONG );
ULONG APIENTRY GpiQueryPS( HPS, PSIZEL );
 
#ifndef INCL_SAADEFS
 
LONG APIENTRY GpiErrorSegmentData( HPS, PLONG, PLONG );
LONG APIENTRY GpiQueryDrawControl( HPS, LONG );
BOOL APIENTRY GpiSetDrawControl( HPS, LONG, LONG );
LONG APIENTRY GpiQueryDrawingMode( HPS );
BOOL APIENTRY GpiSetDrawingMode( HPS, LONG );
LONG APIENTRY GpiQueryStopDraw( HPS );
BOOL APIENTRY GpiSetStopDraw( HPS, LONG );
 
#endif /* no INCL_SAADEFS */
 
#endif /* non-common GPICONTROL */
#ifdef INCL_GPICORRELATION
 
/* options for GpiSetPickApertureSize */
#define PICKAP_DEFAULT          0L
#define PICKAP_REC              2L
 
/* type of correlation for GpiCorrelateChain */
#define PICKSEL_VISIBLE         0L
#define PICKSEL_ALL             1L
 
#ifndef INCL_SAADEFS
/* return code to indicate correlate hit(s) */
#define GPI_HITS    2L
#endif /* no INCL_SAADEFS */
 
/*** picking,  correlation and boundary functions */
LONG APIENTRY GpiCorrelateChain(  HPS, LONG, PPOINTL, LONG, LONG, PLONG );
BOOL APIENTRY GpiQueryTag( HPS, PLONG );
BOOL APIENTRY GpiSetTag( HPS, LONG );
BOOL APIENTRY GpiQueryPickApertureSize( HPS, PSIZEL );
BOOL APIENTRY GpiSetPickApertureSize( HPS, LONG, PSIZEL );
 
#ifndef INCL_SAADEFS
 
BOOL APIENTRY GpiQueryPickAperturePosition( HPS, PPOINTL );
BOOL APIENTRY GpiSetPickAperturePosition( HPS, PPOINTL );
BOOL APIENTRY GpiQueryBoundaryData( HPS, PRECTL );
BOOL APIENTRY GpiResetBoundaryData( HPS );
LONG APIENTRY GpiCorrelateFrom(  HPS, LONG, LONG, LONG, PPOINTL
                              , LONG, LONG, PLONG );
LONG APIENTRY GpiCorrelateSegment( HPS, LONG, LONG, PPOINTL
                                 , LONG, LONG, PLONG );
 
#endif /* no INCL_SAADEFS */
 
#endif /* non-common_GPICORRELATION */
#ifdef INCL_GPISEGMENTS
 
/* data formats for GpiPutData and GpiGetData */
#define DFORM_NOCONV    0L
 
#ifndef INCL_SAADEFS
 
#define DFORM_S370SHORT 1L
#define DFORM_PCSHORT   2L
#define DFORM_PCLONG    4L
 
#endif /* no INCL_SAADEFS */
 
/* segment attributes used by GpiSet/QuerySegmentAttrs and others */
#define ATTR_ERROR           (-1L)
#define ATTR_DETECTABLE      1L
#define ATTR_VISIBLE         2L
#define ATTR_CHAINED         6L
 
#ifndef INCL_SAADEFS
 
#define ATTR_DYNAMIC         8L
#define ATTR_FASTCHAIN       9L
#define ATTR_PROP_DETECTABLE 10L
#define ATTR_PROP_VISIBLE    11L
 
#endif /* no INCL_SAADEFS */
 
/* attribute on/off values */
#define ATTR_OFF  0L
#define ATTR_ON   1L
 
/* segment priority used by GpiSetSegmentPriority and others */
#define LOWER_PRI       (-1L)
#define HIGHER_PRI      1L
 
/*** segment control functions */
BOOL APIENTRY GpiOpenSegment( HPS, LONG );
BOOL APIENTRY GpiCloseSegment( HPS );
BOOL APIENTRY GpiDeleteSegment( HPS, LONG );
LONG APIENTRY GpiQueryInitialSegmentAttrs( HPS, LONG );
BOOL APIENTRY GpiSetInitialSegmentAttrs( HPS, LONG, LONG );
LONG APIENTRY GpiQuerySegmentAttrs( HPS, LONG, LONG );
BOOL APIENTRY GpiSetSegmentAttrs( HPS, LONG, LONG, LONG );
LONG APIENTRY GpiQuerySegmentPriority( HPS, LONG, LONG );
BOOL APIENTRY GpiSetSegmentPriority( HPS, LONG, LONG, LONG );
 
#ifndef INCL_SAADEFS
 
BOOL  APIENTRY GpiDeleteSegments( HPS, LONG, LONG );
LONG  APIENTRY GpiQuerySegmentNames( HPS, LONG, LONG, LONG, PLONG );
 
#endif /* no INCL_SAADEFS */
 
/*** draw functions for segments */
LONG APIENTRY GpiGetData( HPS, LONG, PLONG, LONG, LONG, PBYTE );
LONG APIENTRY GpiPutData( HPS, LONG, PLONG, PBYTE );
 
#ifndef INCL_SAADEFS
 
BOOL APIENTRY GpiDrawChain( HPS );
BOOL APIENTRY GpiDrawFrom( HPS, LONG, LONG );
BOOL APIENTRY GpiDrawSegment( HPS, LONG );
BOOL APIENTRY GpiDrawDynamics( HPS );
BOOL APIENTRY GpiRemoveDynamics( HPS, LONG, LONG );
 
#endif /* no INCL_SAADEFS */
 
#endif /* non-common GPISEGMENTS */
#ifdef INCL_GPISEGEDITING
 
/* edit modes used by GpiSet/QueryEditMode */
#define SEGEM_ERROR     0L
#define SEGEM_INSERT    1L
#define SEGEM_REPLACE   2L
 
/*** segment editing by element functions */
BOOL APIENTRY GpiBeginElement( HPS, LONG, PSZ );
BOOL APIENTRY GpiEndElement( HPS );
BOOL APIENTRY GpiLabel( HPS, LONG );
LONG APIENTRY GpiElement( HPS, LONG, PSZ, LONG, PBYTE );
LONG APIENTRY GpiQueryElement( HPS, LONG, LONG, PBYTE );
BOOL APIENTRY GpiDeleteElement( HPS );
BOOL APIENTRY GpiDeleteElementRange( HPS, LONG, LONG );
BOOL APIENTRY GpiDeleteElementsBetweenLabels( HPS, LONG, LONG );
LONG APIENTRY GpiQueryEditMode( HPS );
BOOL APIENTRY GpiSetEditMode( HPS, LONG );
LONG APIENTRY GpiQueryElementPointer( HPS );
BOOL APIENTRY GpiSetElementPointer( HPS, LONG );
BOOL APIENTRY GpiOffsetElementPointer( HPS, LONG );
LONG APIENTRY GpiQueryElementType( HPS, PLONG, LONG, PSZ );
BOOL APIENTRY GpiSetElementPointerAtLabel( HPS, LONG );
 
#endif /* non-common GPISEGEDITING */
#ifdef INCL_GPITRANSFORMS
 
#ifndef INCL_SAADEFS
 
/* co-ordinates space for GpiConvert */
#define CVTC_WORLD              1L
#define CVTC_MODEL              2L
#define CVTC_DEFAULTPAGE        3L
#define CVTC_PAGE               4L
#define CVTC_DEVICE             5L
 
#endif /* no INCL_SAADEFS */
 
/* type of transformation for GpiSetSegmentTransformMatrix */
#define TRANSFORM_REPLACE       0L
#define TRANSFORM_ADD           1L
#define TRANSFORM_PREEMPT       2L
 
/* transform matrix */
typedef struct _MATRIXLF {     /* matlf */
    FIXED fxM11;
    FIXED fxM12;
    LONG  lM13;
    FIXED fxM21;
    FIXED fxM22;
    LONG  lM23;
    LONG  lM31;
    LONG  lM32;
    LONG  lM33;
} MATRIXLF;
typedef MATRIXLF FAR *PMATRIXLF;
 
#ifndef INCL_DDIDEFS
 
/*** transform and transform conversion functions */
BOOL APIENTRY GpiQuerySegmentTransformMatrix( HPS, LONG, LONG, PMATRIXLF );
BOOL APIENTRY GpiSetSegmentTransformMatrix( HPS, LONG, LONG, PMATRIXLF, LONG );
 
#ifndef INCL_SAADEFS
 
BOOL APIENTRY GpiConvert( HPS, LONG, LONG, LONG, PPOINTL );
BOOL APIENTRY GpiQueryModelTransformMatrix( HPS, LONG, PMATRIXLF );
BOOL APIENTRY GpiSetModelTransformMatrix( HPS, LONG, PMATRIXLF, LONG );
LONG APIENTRY GpiCallSegmentMatrix( HPS, LONG, LONG, PMATRIXLF, LONG );
BOOL APIENTRY GpiQueryDefaultViewMatrix( HPS, LONG, PMATRIXLF );
BOOL APIENTRY GpiSetDefaultViewMatrix( HPS, LONG, PMATRIXLF, LONG );
BOOL APIENTRY GpiQueryPageViewport( HPS, PRECTL );
BOOL APIENTRY GpiSetPageViewport( HPS, PRECTL );
BOOL APIENTRY GpiQueryViewingTransformMatrix( HPS, LONG, PMATRIXLF );
BOOL APIENTRY GpiSetViewingTransformMatrix( HPS, LONG, PMATRIXLF, LONG );
 
#endif /* no INCL_SAADEFS */
 
/*** general clipping functions */
BOOL APIENTRY GpiSetGraphicsField( HPS, PRECTL );
BOOL APIENTRY GpiQueryGraphicsField( HPS, PRECTL );
BOOL APIENTRY GpiSetViewingLimits( HPS, PRECTL );
BOOL APIENTRY GpiQueryViewingLimits( HPS, PRECTL );
 
#endif /* no INCL_DDIDEFS */
 
#endif /* non-common GPITRANSFORMS */
#ifdef INCL_GPIPATHS
 
#ifndef INCL_SAADEFS
 
/* modes for GpiModifyPath */
#define MPATH_STROKE            6L
 
/* modes for GpiFillPath */
#define FPATH_ALTERNATE         0L
#define FPATH_WINDING           2L
 
/* modes for GpiSetClipPath */
#define SCP_ALTERNATE           0L
#define SCP_WINDING             2L
#define SCP_AND                 4L
#define SCP_RESET               0L
 
#ifndef INCL_DDIDEFS
 
/*** Path and Clip Path functions */
BOOL APIENTRY GpiBeginPath( HPS, LONG );
BOOL APIENTRY GpiEndPath( HPS );
BOOL APIENTRY GpiCloseFigure( HPS );
BOOL APIENTRY GpiModifyPath( HPS, LONG, LONG );
LONG APIENTRY GpiFillPath( HPS, LONG, LONG );
BOOL APIENTRY GpiSetClipPath( HPS, LONG, LONG );
LONG APIENTRY GpiStrokePath( HPS, LONG, ULONG );
 
#endif /* no INCL_DDIDEFS */
 
#endif /* no INCL_SAADEFS */
 
#endif /* non-common GPIPATHS */
#ifdef INCL_GPILOGCOLORTABLE
 
#ifndef INCL_SAADEFS
 
/* options for GpiCreateLogColorTable and others */
#define LCOL_RESET              0x0001L
#define LCOL_REALIZABLE         0x0002L
#define LCOL_PURECOLOR          0x0004L
 
/* format of logical lColor table for GpiCreateLogColorTable and others */
#define LCOLF_DEFAULT           0L
#define LCOLF_INDRGB            1L
#define LCOLF_CONSECRGB         2L
#define LCOLF_RGB               3L
 
/* options for GpiQueryRealColors and others */
#define LCOLOPT_REALIZED        0x0001L
#define LCOLOPT_INDEX           0x0002L
 
/* return codes from GpiQueryLogColorTable to indicate it is in RGB mode */
#define QLCT_ERROR   (-1L)
#define QLCT_RGB     (-2L)
 
/* GpiQueryLogColorTable index returned for colors not explicitly loaded */
#define QLCT_NOTLOADED (-1L)
 
/* return code on GpiQueryRealColours */
#define QRC_NOINDEX  (-254L)
 
/* return codes for GpiQueryColorData */
#define QCD_LCT_FORMAT    0L
#define QCD_LCT_LOINDEX   1L
#define QCD_LCT_HIINDEX   2L
 
#ifndef INCL_DDIDEFS
 
/*** logical lColor table functions */
BOOL APIENTRY GpiCreateLogColorTable( HPS, ULONG, LONG, LONG, LONG, PLONG );
BOOL APIENTRY GpiRealizeColorTable( HPS );
BOOL APIENTRY GpiUnrealizeColorTable( HPS );
BOOL APIENTRY GpiQueryColorData( HPS, LONG, PLONG );
LONG APIENTRY GpiQueryLogColorTable( HPS, ULONG, LONG, LONG, PLONG );
LONG APIENTRY GpiQueryRealColors( HPS, ULONG, LONG, LONG, PLONG );
LONG APIENTRY GpiQueryNearestColor( HPS, ULONG, LONG );
LONG APIENTRY GpiQueryColorIndex( HPS, ULONG, LONG );
LONG APIENTRY GpiQueryRGBColor( HPS, ULONG, LONG );
 
#endif /* no INCL_DDIDEFS */
 
#endif /* no INCL_SAADEFS */
 
#endif /* non-common GPILOGCOLORTABLE */
#ifdef INCL_GPIPRIMITIVES
 
/* default color table indices */
 
#ifndef INCL_SAADEFS
#define CLR_FALSE               (-5L)
#define CLR_TRUE                (-4L)
#endif /* no INCL_SAADEFS */
 
#define CLR_ERROR               (-255L)
#define CLR_DEFAULT             (-3L)
#define CLR_WHITE               (-2L)
#define CLR_BLACK               (-1L)
#define CLR_BACKGROUND          0L
#define CLR_BLUE                1L
#define CLR_RED                 2L
#define CLR_PINK                3L
#define CLR_GREEN               4L
#define CLR_CYAN                5L
#define CLR_YELLOW              6L
#define CLR_NEUTRAL             7L
 
#ifndef INCL_SAADEFS
 
#define CLR_DARKGRAY            8L
#define CLR_PALEBLUE            9L
#define CLR_PALERED            10L
#define CLR_PALEPINK           11L
#define CLR_DARKGREEN          12L
#define CLR_DARKCYAN           13L
#define CLR_BROWN              14L
#define CLR_PALEGRAY           15L
 
/* rgb colors */
#define RGB_ERROR               (-255L)
#define RGB_BLACK               0x00000000L
#define RGB_BLUE                0x000000FFL
#define RGB_GREEN               0x0000FF00L
#define RGB_CYAN                0x0000FFFFL
#define RGB_RED                 0x00FF0000L
#define RGB_PINK                0x00FF00FFL
#define RGB_YELLOW              0x00FFFF00L
#define RGB_WHITE               0x00FFFFFFL
#endif /* no INCL_SAADEFS */
 
/* control flags used by GpiBeginArea */
#define BA_NOBOUNDARY   0L
#define BA_BOUNDARY     0x0001L
 
#ifndef INCL_SAADEFS
#define BA_ALTERNATE    0L
#define BA_WINDING      0x0002L
 
/* fill options for GpiBox/GpiFullArc */
#define DRO_FILL        1L
#define DRO_OUTLINE     2L
#define DRO_OUTLINEFILL 3L
 
#endif /* no INCL_SAADEFS */
 
/* basic pattern symbols */
#define PATSYM_ERROR           (-1L)
#define PATSYM_DEFAULT         0L
#define PATSYM_DENSE1          1L
#define PATSYM_DENSE2          2L
#define PATSYM_DENSE3          3L
#define PATSYM_DENSE4          4L
#define PATSYM_DENSE5          5L
#define PATSYM_DENSE6          6L
#define PATSYM_DENSE7          7L
#define PATSYM_DENSE8          8L
#define PATSYM_VERT            9L
#define PATSYM_HORIZ           10L
#define PATSYM_DIAG1           11L
#define PATSYM_DIAG2           12L
#define PATSYM_DIAG3           13L
#define PATSYM_DIAG4           14L
#define PATSYM_NOSHADE         15L
#define PATSYM_SOLID           16L
#define PATSYM_BLANK           64L
#ifndef INCL_SAADEFS
#define PATSYM_HALFTONE        17L
#endif /* no INCL_SAADEFS */
 
/* lcid values for GpiSet/QueryPattern and others */
#define LCID_ERROR             (-1L)
#define LCID_DEFAULT           0L
 
#ifndef INCL_DDIDEFS
 
/*** global primitive functions */
BOOL APIENTRY GpiSetColor( HPS, LONG );
LONG APIENTRY GpiQueryColor( HPS );
 
/*** line primitive functions */
#ifndef INCL_SAADEFS
LONG APIENTRY GpiBox( HPS, LONG, PPOINTL, LONG, LONG );
#endif /* no INCL_SAADEFS */
 
BOOL APIENTRY GpiMove( HPS, PPOINTL );
LONG APIENTRY GpiLine( HPS, PPOINTL );
LONG APIENTRY GpiPolyLine( HPS, LONG, PPOINTL );
 
/*** area primitive functions */
BOOL APIENTRY GpiSetPattern( HPS, LONG );
LONG APIENTRY GpiQueryPattern( HPS );
BOOL APIENTRY GpiBeginArea( HPS, ULONG );
LONG APIENTRY GpiEndArea( HPS );
 
/*** character primitive functions */
LONG APIENTRY GpiCharString( HPS, LONG, PCH );
LONG APIENTRY GpiCharStringAt( HPS, PPOINTL, LONG, PCH );
 
#endif /* no INCL_DDIDEFS */
 
#endif /* common GPIPRIMTIVES */
#ifdef INCL_GPIPRIMITIVES
 
/* mode for GpiSetAttrMode */
#define AM_ERROR          (-1L)
#define AM_PRESERVE       0L
#define AM_NOPRESERVE     1L
 
/* foreground mixes */
#define FM_ERROR               (-1L)
#define FM_DEFAULT             0L
#define FM_OR                  1L
#define FM_OVERPAINT           2L
#define FM_LEAVEALONE          5L
 
#ifndef INCL_SAADEFS
 
#define FM_XOR                 4L
#define FM_AND                 6L
#define FM_SUBTRACT            7L
#define FM_MASKSRCNOT          8L
#define FM_ZERO                9L
#define FM_NOTMERGESRC         10L
#define FM_NOTXORSRC           11L
#define FM_INVERT              12L
#define FM_MERGESRCNOT         13L
#define FM_NOTCOPYSRC          14L
#define FM_MERGENOTSRC         15L
#define FM_NOTMASKSRC          16L
#define FM_ONE                 17L
 
#endif /* no INCL_SAADEFS */
 
/* background mixes */
#define BM_ERROR               (-1L)
#define BM_DEFAULT             0L
#define BM_OVERPAINT           2L
#define BM_LEAVEALONE          5L
 
#ifndef INCL_SAADEFS
#define BM_OR                  1L
#define BM_XOR                 4L
#endif /* no INCL_SAADEFS */
 
/* basic line type styles */
#define LINETYPE_ERROR          (-1L)
#define LINETYPE_DEFAULT        0L
#define LINETYPE_DOT            1L
#define LINETYPE_SHORTDASH      2L
#define LINETYPE_DASHDOT        3L
#define LINETYPE_DOUBLEDOT      4L
#define LINETYPE_LONGDASH       5L
#define LINETYPE_DASHDOUBLEDOT  6L
#define LINETYPE_SOLID          7L
#define LINETYPE_INVISIBLE      8L
#ifndef INCL_SAADEFS
#define LINETYPE_ALTERNATE      9L
#endif /* no INCL_SAADEFS */
 
/* cosmetic line widths */
#define LINEWIDTH_ERROR         (-1L)
#define LINEWIDTH_DEFAULT       0L
#define LINEWIDTH_NORMAL        0x00010000L   /* MAKEFIXED(1,0) */
 
#ifndef INCL_SAADEFS
 
/* actual line widths */
#define LINEWIDTHGEOM_ERROR     (-1L)
 
/* line end styles */
#define LINEEND_ERROR           (-1L)
#define LINEEND_DEFAULT         0L
#define LINEEND_FLAT            1L
#define LINEEND_SQUARE          2L
#define LINEEND_ROUND           3L
 
/* line join styles */
#define LINEJOIN_ERROR          (-1L)
#define LINEJOIN_DEFAULT        0L
#define LINEJOIN_BEVEL          1L
#define LINEJOIN_ROUND          2L
#define LINEJOIN_MITRE          3L
 
#endif /* no INCL_SAADEFS */
 
/* charaction directions */
#define CHDIRN_ERROR            (-1L)
#define CHDIRN_DEFAULT          0L
#define CHDIRN_LEFTRIGHT        1L
#define CHDIRN_TOPBOTTOM        2L
#define CHDIRN_RIGHTLEFT        3L
#define CHDIRN_BOTTOMTOP        4L
 
/* character modes */
#define CM_ERROR                (-1L)
#define CM_DEFAULT              0L
#define CM_MODE1                1L
#define CM_MODE2                2L
#define CM_MODE3                3L
 
/* basic marker symbols */
#define MARKSYM_ERROR           (-1L)
#define MARKSYM_DEFAULT         0L
#define MARKSYM_CROSS           1L
#define MARKSYM_PLUS            2L
#define MARKSYM_DIAMOND         3L
#define MARKSYM_SQUARE          4L
#define MARKSYM_SIXPOINTSTAR    5L
#define MARKSYM_EIGHTPOINTSTAR  6L
#define MARKSYM_SOLIDDIAMOND    7L
#define MARKSYM_SOLIDSQUARE     8L
#define MARKSYM_DOT             9L
#define MARKSYM_SMALLCIRCLE     10L
#define MARKSYM_BLANK           64L
 
#ifndef INCL_SAADEFS
 
/* formatting options for GpiCharStringPosAt */
#define CHS_OPAQUE              0x0001L
#define CHS_VECTOR              0x0002L
#define CHS_LEAVEPOS            0x0008L
#define CHS_CLIP                0x0010L
 
/* bundle codes for GpiSetAttributes and GpiQueryAttributes */
#define PRIM_LINE               1L
#define PRIM_CHAR               2L
#define PRIM_MARKER             3L
#define PRIM_AREA               4L
#define PRIM_IMAGE              5L
 
/* line bundle mask bits */
#define LBB_COLOR               0x0001L
#define LBB_MIX_MODE            0x0004L
#define LBB_WIDTH               0x0010L
#define LBB_GEOM_WIDTH          0x0020L
#define LBB_TYPE                0x0040L
#define LBB_END                 0x0080L
#define LBB_JOIN                0x0100L
 
/* character bundle mask bits */
#define CBB_COLOR               0x0001L
#define CBB_BACK_COLOR          0x0002L
#define CBB_MIX_MODE            0x0004L
#define CBB_BACK_MIX_MODE       0x0008L
#define CBB_SET                 0x0010L
#define CBB_MODE                0x0020L
#define CBB_BOX                 0x0040L
#define CBB_ANGLE               0x0080L
#define CBB_SHEAR               0x0100L
#define CBB_DIRECTION           0x0200L
 
/* marker bundle mask bits */
#define MBB_COLOR               0x0001L
#define MBB_BACK_COLOR          0x0002L
#define MBB_MIX_MODE            0x0004L
#define MBB_BACK_MIX_MODE       0x0008L
#define MBB_SET                 0x0010L
#define MBB_SYMBOL              0x0020L
#define MBB_BOX                 0x0040L
 
/* pattern bundle mask bits */
#define ABB_COLOR               0x0001L
#define ABB_BACK_COLOR          0x0002L
#define ABB_MIX_MODE            0x0004L
#define ABB_BACK_MIX_MODE       0x0008L
#define ABB_SET                 0x0010L
#define ABB_SYMBOL              0x0020L
#define ABB_REF_POINT           0x0040L
 
/* image bundle mask bits */
#define IBB_COLOR               0x0001L
#define IBB_BACK_COLOR          0x0002L
#define IBB_MIX_MODE            0x0004L
#define IBB_BACK_MIX_MODE       0x0008L
 
/* structure for GpiSetArcParams and GpiQueryArcParams */
typedef struct _ARCPARAMS {    /* arcp */
    LONG lP;
    LONG lQ;
    LONG lR;
    LONG lS;
} ARCPARAMS;
typedef ARCPARAMS FAR *PARCPARAMS;
 
#endif /* no INCL_SAADEFS */
 
/* variation of SIZE used for FIXEDs */
typedef struct _SIZEF {       /* sizfx */
    FIXED cx;
    FIXED cy;
} SIZEF;
typedef SIZEF FAR *PSIZEF;
 
/* structure for gradient parameters e.g. for GpiSetCharAngle */
typedef struct _GRADIENTL {     /* gradl */
    LONG x;
    LONG y;
} GRADIENTL;
typedef GRADIENTL FAR *PGRADIENTL;
 
#ifndef INCL_SAADEFS
 
/* line bundle for GpiSetAttributes and GpiQueryAttributes */
typedef struct _LINEBUNDLE {    /* lbnd */
    LONG    lColor;
    LONG    lReserved;
    USHORT  usMixMode;
    USHORT  usReserved;
    FIXED   fxWidth;
    LONG    lGeomWidth;
    USHORT  usType;
    USHORT  usEnd;
    USHORT  usJoin;
} LINEBUNDLE;
 
/* character bundle for GpiSetAttributes and GpiQueryAttributes */
typedef struct _CHARBUNDLE {    /* cbnd */
    LONG      lColor;
    LONG      lBackColor;
    USHORT    usMixMode;
    USHORT    usBackMixMode;
    USHORT    usSet;
    USHORT    usPrecision;
    SIZEF     sizfxCell;
    POINTL    ptlAngle;
    POINTL    ptlShear;
    USHORT    usDirection;
} CHARBUNDLE;
 
/* marker bundle for GpiSetAttributes and GpiQueryAttributes */
typedef struct _MARKERBUNDLE {  /* mbnd */
    LONG   lColor;
    LONG   lBackColor;
    USHORT usMixMode;
    USHORT usBackMixMode;
    USHORT usSet;
    USHORT usSymbol;
    SIZEF  sizfxCell;
} MARKERBUNDLE;
 
/* pattern bundle for GpiSetAttributes and GpiQueryAttributes */
typedef struct _AREABUNDLE { /* pbnd */
    LONG   lColor;
    LONG   lBackColor;
    USHORT usMixMode;
    USHORT usBackMixMode;
    USHORT usSet;
    USHORT usSymbol;
    POINTL ptlRefPoint ;
} AREABUNDLE;
 
/* image bundle for GpiSetAttributes and GpiQueryAttributes */
typedef struct _IMAGEBUNDLE {   /* ibmd */
    LONG   lColor;
    LONG   lBackColor;
    USHORT usMixMode;
    USHORT usBackMixMode;
} IMAGEBUNDLE;
 
/* pointer to any bundle used by GpiSet/QueryAttrs */
typedef PVOID PBUNDLE;
 
/* array indices for GpiQueryTextBox */
#define TXTBOX_TOPLEFT      0L
#define TXTBOX_BOTTOMLEFT   1L
#define TXTBOX_TOPRIGHT     2L
#define TXTBOX_BOTTOMRIGHT  3L
#define TXTBOX_CONCAT       4L
/* array count for GpiQueryTextBox */
#define TXTBOX_COUNT        5L
 
/* return codes for GpiPtVisible */
#define PVIS_ERROR       0L
#define PVIS_INVISIBLE   1L
#define PVIS_VISIBLE     2L
 
/* return codes for GpiRectVisible */
#define RVIS_ERROR       0L
#define RVIS_INVISIBLE   1L
#define RVIS_PARTIAL     2L
#define RVIS_VISIBLE     3L
 
#endif /* no INCL_SAADEFS */
 
#ifndef INCL_DDIDEFS
 
/*** attribute mode functions */
BOOL APIENTRY GpiSetAttrMode( HPS, LONG );
LONG APIENTRY GpiQueryAttrMode( HPS );
/*** bundle primitive functions */
#ifndef INCL_SAADEFS
 
BOOL APIENTRY GpiSetAttrs( HPS, LONG, ULONG, ULONG, PBUNDLE );
LONG APIENTRY GpiQueryAttrs( HPS, LONG, ULONG, PBUNDLE );
 
#endif /* no INCL_SAADEFS */
 
/*** global primitive functions */
BOOL APIENTRY GpiSetBackColor( HPS, LONG );
LONG APIENTRY GpiQueryBackColor( HPS );
BOOL APIENTRY GpiSetMix( HPS, LONG );
LONG APIENTRY GpiQueryMix( HPS );
BOOL APIENTRY GpiSetBackMix( HPS, LONG );
LONG APIENTRY GpiQueryBackMix( HPS );
 
/*** line primitive functions */
BOOL  APIENTRY GpiSetLineType( HPS, LONG );
LONG  APIENTRY GpiQueryLineType( HPS );
BOOL  APIENTRY GpiSetLineWidth( HPS, FIXED );
FIXED APIENTRY GpiQueryLineWidth( HPS );
 
#ifndef INCL_SAADEFS
 
BOOL APIENTRY GpiSetLineWidthGeom( HPS, LONG );
LONG APIENTRY GpiQueryLineWidthGeom( HPS );
BOOL APIENTRY GpiSetLineEnd( HPS, LONG );
LONG APIENTRY GpiQueryLineEnd( HPS );
BOOL APIENTRY GpiSetLineJoin( HPS, LONG );
LONG APIENTRY GpiQueryLineJoin( HPS );
 
#endif /* no INCL_SAADEFS */
 
BOOL APIENTRY GpiSetCurrentPosition( HPS, PPOINTL );
BOOL APIENTRY GpiQueryCurrentPosition( HPS, PPOINTL );
/*** arc primitive functions */
#ifndef INCL_SAADEFS
 
BOOL APIENTRY GpiSetArcParams( HPS, PARCPARAMS );
BOOL APIENTRY GpiQueryArcParams( HPS, PARCPARAMS );
LONG APIENTRY GpiPointArc( HPS, PPOINTL );
LONG APIENTRY GpiFullArc( HPS, LONG, FIXED );
LONG APIENTRY GpiPartialArc( HPS, PPOINTL, FIXED, FIXED, FIXED );
LONG APIENTRY GpiPolyFilletSharp( HPS, LONG, PPOINTL, PFIXED );
LONG APIENTRY GpiPolySpline( HPS, LONG, PPOINTL );
 
#endif /* no INCL_SAADEFS */
 
LONG APIENTRY GpiPolyFillet( HPS, LONG, PPOINTL );
/*** area primitive functions */
BOOL APIENTRY GpiSetPatternSet( HPS, LONG );
LONG APIENTRY GpiQueryPatternSet( HPS );
 
#ifndef INCL_SAADEFS
BOOL APIENTRY GpiSetPatternRefPoint( HPS, PPOINTL );
BOOL APIENTRY GpiQueryPatternRefPoint( HPS, PPOINTL );
#endif /* no INCL_SAADEFS */
 
/*** character primitive functions */
 
#ifndef INCL_SAADEFS
 
BOOL APIENTRY GpiQueryCharStringPos( HPS, ULONG, LONG, PCH, PLONG, PPOINTL );
BOOL APIENTRY GpiQueryCharStringPosAt( HPS, PPOINTL, ULONG, LONG, PCH, PLONG, PPOINTL );
 
#endif /* no INCL_SAADEFS */
 
BOOL APIENTRY GpiQueryTextBox( HPS, LONG, PCH, LONG, PPOINTL );
BOOL APIENTRY GpiQueryDefCharBox( HPS, PSIZEL );
BOOL APIENTRY GpiSetCharSet( HPS, LONG );
LONG APIENTRY GpiQueryCharSet( HPS );
BOOL APIENTRY GpiSetCharBox( HPS, PSIZEF );
BOOL APIENTRY GpiQueryCharBox( HPS, PSIZEF );
BOOL APIENTRY GpiSetCharAngle( HPS, PGRADIENTL );
BOOL APIENTRY GpiQueryCharAngle( HPS, PGRADIENTL );
BOOL APIENTRY GpiSetCharShear( HPS, PPOINTL );
BOOL APIENTRY GpiQueryCharShear( HPS, PPOINTL );
BOOL APIENTRY GpiSetCharDirection( HPS, LONG );
LONG APIENTRY GpiQueryCharDirection( HPS );
BOOL APIENTRY GpiSetCharMode( HPS, LONG );
LONG APIENTRY GpiQueryCharMode( HPS );
 
#ifndef INCL_SAADEFS
 
LONG APIENTRY GpiCharStringPos( HPS, PRECTL, ULONG, LONG, PCH, PLONG );
LONG APIENTRY GpiCharStringPosAt( HPS, PPOINTL, PRECTL
                                 , ULONG, LONG, PCH ,PLONG );
#endif /* no INCL_SAADEFS */
 
/*** marker primitive functions */
BOOL APIENTRY GpiSetMarkerSet( HPS, LONG );
LONG APIENTRY GpiQueryMarkerSet( HPS );
BOOL APIENTRY GpiSetMarker( HPS, LONG );
LONG APIENTRY GpiQueryMarker( HPS );
BOOL APIENTRY GpiSetMarkerBox( HPS, PSIZEF );
BOOL APIENTRY GpiQueryMarkerBox( HPS, PSIZEF );
LONG APIENTRY GpiMarker( HPS, PPOINTL );
LONG APIENTRY GpiPolyMarker( HPS, LONG, PPOINTL );
/*** image primitive functions */
LONG APIENTRY GpiImage( HPS, LONG, PSIZEL, LONG, PBYTE );
/*** micellaneous primitive functions */
BOOL  APIENTRY GpiPop( HPS, LONG );
 
#ifndef INCL_SAADEFS
 
LONG APIENTRY GpiPtVisible( HPS, PPOINTL );
LONG APIENTRY GpiRectVisible( HPS, PRECTL );
BOOL  APIENTRY GpiComment( HPS, LONG, PBYTE );
 
#endif /* no INCL_SAADEFS */
 
#endif /* no INCL_DDIDEFS */
 
#endif /* non-common GPIPRIMITIVES */
#ifdef INCL_GPILCIDS
 
/* lcid type for GpiQuerySetIds */
#define LCIDT_FONT       6L
 
#ifndef INCL_SAADEFS
#define LCIDT_BITMAP     7L
 
/* constant used to delete all lcids by GpiDeleteSetId */
#define LCID_ALL         (-1L)
#endif /* no INCL_SAADEFS */
 
/* kerning data returned by GpiQueryKerningPairs */
typedef struct _KERNINGPAIRS {  /* krnpr */
    SHORT sFirstChar;
    SHORT sSecondChar;
    SHORT sKerningAmount;
} KERNINGPAIRS;
typedef KERNINGPAIRS FAR *PKERNINGPAIRS;
 
#ifndef INCL_SAADEFS
 
/* options for GpiQueryFonts */
#define QF_PUBLIC               0x0001L
#define QF_PRIVATE              0x0002L
 
/* font file descriptions for GpiQueryFontFileDescriptions */
typedef CHAR FFDESCS[2][FACESIZE]; /* ffdescs */
typedef FFDESCS FAR *PFFDESCS;
 
#endif /* no INCL_SAADEFS */
 
#ifndef INCL_DDIDEFS
 
/*** physical and logical font functions */
BOOL APIENTRY GpiDeleteSetId( HPS, LONG );
LONG APIENTRY GpiQueryNumberSetIds( HPS );
BOOL APIENTRY GpiQuerySetIds( HPS, LONG, PLONG, PSTR8, PLONG );
BOOL APIENTRY GpiLoadFonts( HAB, PSZ );
BOOL APIENTRY GpiUnloadFonts( HAB, PSZ );
LONG APIENTRY GpiCreateLogFont( HPS, PSTR8, LONG, PFATTRS );
LONG APIENTRY GpiQueryFonts( HPS, ULONG, PSZ, PLONG, LONG, PFONTMETRICS );
BOOL APIENTRY GpiQueryFontMetrics( HPS, LONG, PFONTMETRICS );
LONG APIENTRY GpiQueryKerningPairs( HPS, LONG, PKERNINGPAIRS );
BOOL APIENTRY GpiQueryWidthTable( HPS, LONG, LONG, PLONG );
#ifndef INCL_SAADEFS
BOOL   APIENTRY GpiSetCp( HPS, USHORT );
USHORT APIENTRY GpiQueryCp( HPS );
LONG   APIENTRY GpiQueryFontFileDescriptions( HAB, PSZ, PLONG, PFFDESCS );
#endif /* no INCL_SAADEFS */
 
#endif /* no INCL_DDIDEFS */
 
#endif /* non-common GPILCIDS */
#ifdef INCL_GPIBITMAPS
 
/* rastor operations defined for GpiBitBlt */
#define ROP_SRCCOPY             0x00CCL
#define ROP_SRCPAINT            0x00EEL
#define ROP_SRCAND              0x0088L
#define ROP_SRCINVERT           0x0066L
#define ROP_SRCERASE            0x0044L
#define ROP_NOTSRCCOPY          0x0033L
#define ROP_NOTSRCERASE         0x0011L
#define ROP_MERGECOPY           0x00C0L
#define ROP_MERGEPAINT          0x00BBL
#define ROP_PATCOPY             0x00F0L
#define ROP_PATPAINT            0x00FBL
#define ROP_PATINVERT           0x005AL
#define ROP_DSTINVERT           0x0055L
#define ROP_ZERO                0x0000L
#define ROP_ONE                 0x00FFL
 
/* Blt options for GpiBitBlt */
#define BBO_OR              0L
#define BBO_AND             1L
#define BBO_IGNORE          2L
 
/* error return for GpiSetBitmap */
#define HBM_ERROR               ((HBITMAP)-1L)
 
#ifndef INCL_DDIDEFS
 
/*** bitmap and pel functions */
HBITMAP APIENTRY GpiLoadBitmap( HPS, USHORT, USHORT, LONG, LONG );
BOOL    APIENTRY GpiDeleteBitmap( HBITMAP );
HBITMAP APIENTRY GpiSetBitmap( HPS, HBITMAP );
LONG    APIENTRY GpiBitBlt( HPS, HPS, LONG, PPOINTL, LONG, LONG );
LONG    APIENTRY GpiWCBitBlt( HPS, HBITMAP, LONG, PPOINTL, LONG, LONG );
 
#endif /* no INCL_DDIDEFS */
 
#endif /* common GPIBITMAPS */
#ifdef INCL_GPIBITMAPS
 
/* usage flags for GpiCreateBitmap */
#define CBM_INIT        0x0004L
 
/* bitmap parameterization used by GpiCreateBitmap and others */
typedef struct _BITMAPINFOHEADER {      /* bmp */
    ULONG  cbFix;
    USHORT cx;
    USHORT cy;
    USHORT cPlanes;
    USHORT cBitCount;
} BITMAPINFOHEADER;
typedef BITMAPINFOHEADER FAR *PBITMAPINFOHEADER;
 
/* RGB data for _BITMAPINFO struct */
typedef struct _RGB {           /* rgb */
    BYTE bBlue;
    BYTE bGreen;
    BYTE bRed;
} RGB;
 
/* bitmap data used by GpiSetBitmapBits and others */
typedef struct _BITMAPINFO {    /* bmi */
    ULONG  cbFix;
    USHORT cx;
    USHORT cy;
    USHORT cPlanes;
    USHORT cBitCount;
    RGB    argbColor[1];
} BITMAPINFO;
typedef BITMAPINFO FAR *PBITMAPINFO;
 
/* error return code for GpiSet/QueryBitmapBits */
#define BMB_ERROR       (-1L)
 
#ifndef INCL_DDIDEFS
 
/*** bitmap and pel functions */
HBITMAP APIENTRY GpiCreateBitmap( HPS, PBITMAPINFOHEADER
                                , ULONG, PBYTE, PBITMAPINFO );
BOOL    APIENTRY GpiSetBitmapDimension( HBITMAP, PSIZEL );
BOOL    APIENTRY GpiQueryBitmapDimension( HBITMAP, PSIZEL );
BOOL    APIENTRY GpiQueryDeviceBitmapFormats( HPS, LONG, PLONG );
BOOL    APIENTRY GpiQueryBitmapParameters( HBITMAP, PBITMAPINFOHEADER );
LONG    APIENTRY GpiQueryBitmapBits( HPS, LONG, LONG, PBYTE, PBITMAPINFO );
LONG    APIENTRY GpiSetBitmapBits( HPS, LONG, LONG, PBYTE, PBITMAPINFO );
LONG    APIENTRY GpiSetPel( HPS, PPOINTL );
LONG    APIENTRY GpiQueryPel( HPS, PPOINTL );
BOOL    APIENTRY GpiSetBitmapId( HPS, HBITMAP, LONG );
HBITMAP APIENTRY GpiQueryBitmapHandle( HPS, LONG );
 
#endif /* no INCL_DDIDEFS */
 
#endif /* non-common GPIBITMAPS */
#ifdef INCL_GPIREGIONS
 
/* options for GpiCombineRegion */
#define CRGN_OR                 1L
#define CRGN_COPY               2L
#define CRGN_XOR                4L
#define CRGN_AND                6L
#define CRGN_DIFF               7L
 
/* usDirection of returned region data for GpiQueryRegionRects */
#define RECTDIR_LFRT_TOPBOT     1L
#define RECTDIR_RTLF_TOPBOT     2L
#define RECTDIR_LFRT_BOTTOP     3L
#define RECTDIR_RTLF_BOTTOP     4L
 
/* control data for GpiQueryRegionRects */
typedef struct _RGNRECT {       /* rgnrc */
    USHORT ircStart;
    USHORT crc;
    USHORT crcReturned;
    USHORT usDirection;
} RGNRECT;
typedef RGNRECT FAR *PRGNRECT;
 
/* return code to indicate type of region for GpiCombineRegion and others */
#define RGN_ERROR       0L
#define RGN_NULL        1L
#define RGN_RECT        2L
#define RGN_COMPLEX     3L
 
/* return codes for GpiPtInRegion */
#define PRGN_ERROR      0L
#define PRGN_OUTSIDE    1L
#define PRGN_INSIDE     2L
 
/* return codes for GpiRectInRegion */
#define RRGN_ERROR      0L
#define RRGN_OUTSIDE    1L
#define RRGN_PARTIAL    2L
#define RRGN_INSIDE     3L
 
/* return codes for GpiEqualRegion */
#define EQRGN_ERROR     0L
#define EQRGN_NOTEQUAL  1L
#define EQRGN_EQUAL     2L
 
/* error return code for GpiSetRegion */
#define HRGN_ERROR      ((HRGN)-1L)
 
#ifndef INCL_DDIDEFS
 
/*** main region functions */
HRGN APIENTRY GpiCreateRegion( HPS, LONG, PRECTL );
BOOL APIENTRY GpiSetRegion( HPS, HRGN, LONG, PRECTL );
BOOL APIENTRY GpiDestroyRegion( HPS, HRGN );
LONG APIENTRY GpiCombineRegion( HPS, HRGN, HRGN, HRGN, LONG );
LONG APIENTRY GpiEqualRegion( HPS, HRGN, HRGN );
BOOL APIENTRY GpiOffsetRegion( HPS, HRGN, PPOINTL );
LONG APIENTRY GpiPtInRegion( HPS, HRGN, PPOINTL );
LONG APIENTRY GpiRectInRegion( HPS, HRGN, PRECTL );
LONG APIENTRY GpiQueryRegionBox( HPS, HRGN, PRECTL );
BOOL APIENTRY GpiQueryRegionRects( HPS, HRGN, PRECTL, PRGNRECT, PRECTL );
LONG APIENTRY GpiPaintRegion( HPS, HRGN );
 
/*** clip region functions */
HRGN APIENTRY GpiSetClipRegion( HPS, HRGN );
HRGN APIENTRY GpiQueryClipRegion( HPS );
LONG APIENTRY GpiQueryClipBox( HPS, PRECTL );
LONG APIENTRY GpiIntersectClipRectangle( HPS, PRECTL );
LONG APIENTRY GpiExcludeClipRectangle( HPS, PRECTL );
LONG APIENTRY GpiOffsetClipRegion( HPS, PPOINTL );
 
#endif /* no INCL_DDIDEFS */
 
#endif /* non-common GPIREGIONS */
#ifdef INCL_GPIMETAFILES
 
#ifndef INCL_DDIDEFS
 
/* options for GpiPlayMetafile */
#define RS_DEFAULT      0L
#define RS_NODISCARD    1L
#define LC_DEFAULT      0L
#define LC_NOLOAD       1L
#define LC_LOADDISC     3L
#define LT_DEFAULT      0L
#define LT_NOMODIFY     1L
 
#ifndef INCL_SAADEFS
 
#define LT_ORIGINALVIEW 4L
#define RES_DEFAULT     0L
#define RES_NORESET     1L
#define RES_RESET       2L
#define SUP_DEFAULT     0L
#define SUP_NOSUPPRESS  1L
#define SUP_SUPPRESS    2L
#define CTAB_DEFAULT    0L
#define CTAB_NOMODIFY   1L
#define CTAB_REPLACE    3L
#define CREA_DEFAULT    0L
#define CREA_REALIZE    1L
#define CREA_NOREALIZE  2L
#define RSP_DEFAULT     0L
#define RSP_NODISCARD   1L
 
#endif /* no INCL_SAADEFS */
 
/*** metafile functions */
HMF   APIENTRY GpiLoadMetaFile( HAB, PSZ );
LONG  APIENTRY GpiPlayMetaFile( HPS, HMF, LONG, PLONG, PLONG, LONG, PSZ );
BOOL  APIENTRY GpiSaveMetaFile( HMF, PSZ );
BOOL  APIENTRY GpiDeleteMetaFile( HMF );
BOOL  APIENTRY GpiAccessMetaFile( HMF );
 
#ifndef INCL_SAADEFS
 
HMF   APIENTRY GpiCopyMetaFile( HMF );
LONG  APIENTRY GpiQueryMetaFileLength( HMF );
BOOL  APIENTRY GpiQueryMetaFileBits( HMF, LONG, LONG, PBYTE );
BOOL  APIENTRY GpiSetMetaFileBits( HMF, LONG, LONG, PBYTE );
 
#endif /* no INCL_SAADEFS */
 
#endif /* no INCL_DDIDEFS */
 
#endif /* non-common GPIMETAFILES */
 
#ifdef INCL_GPIERRORS
 
/* AAB error codes for the GPI */
#define PMERR_OK                           0x0000
#define PMERR_ALREADY_IN_AREA              0x2001
#define PMERR_ALREADY_IN_ELEMENT           0x2002
#define PMERR_ALREADY_IN_PATH              0x2003
#define PMERR_ALREADY_IN_SEG               0x2004
#define PMERR_AREA_INCOMPLETE              0x2005
#define PMERR_BASE_ERROR                   0x2006
#define PMERR_BITBLT_LENGTH_EXCEEDED       0x2007
#define PMERR_BITMAP_IN_USE                0x2008
#define PMERR_BITMAP_IS_SELECTED           0x2009
#define PMERR_BITMAP_NOT_FOUND             0x200A
#define PMERR_BITMAP_NOT_SELECTED          0x200B
#define PMERR_BOUNDS_OVERFLOW              0x200C
#define PMERR_CALLED_SEG_IS_CHAINED        0x200D
#define PMERR_CALLED_SEG_IS_CURRENT        0x200E
#define PMERR_CALLED_SEG_NOT_FOUND         0x200F
#define PMERR_CANNOT_DELETE_ALL_DATA       0x2010
#define PMERR_CANNOT_REPLACE_ELEMENT_0     0x2011
#define PMERR_COL_TABLE_NOT_REALIZABLE     0x2012
#define PMERR_COL_TABLE_NOT_REALIZED       0x2013
#define PMERR_COORDINATE_OVERFLOW          0x2014
#define PMERR_CORR_FORMAT_MISMATCH         0x2015
#define PMERR_DATA_TOO_LONG                0x2016
#define PMERR_DC_IS_ASSOCIATED             0x2017
#define PMERR_DESC_STRING_TRUNCATED        0x2018
#define PMERR_DEVICE_DRIVER_ERROR_1        0x2019
#define PMERR_DEVICE_DRIVER_ERROR_2        0x201A
#define PMERR_DEVICE_DRIVER_ERROR_3        0x201B
#define PMERR_DEVICE_DRIVER_ERROR_4        0x201C
#define PMERR_DEVICE_DRIVER_ERROR_5        0x201D
#define PMERR_DEVICE_DRIVER_ERROR_6        0x201E
#define PMERR_DEVICE_DRIVER_ERROR_7        0x201F
#define PMERR_DEVICE_DRIVER_ERROR_8        0x2020
#define PMERR_DEVICE_DRIVER_ERROR_9        0x2021
#define PMERR_DEVICE_DRIVER_ERROR_10       0x2022
#define PMERR_DOSOPEN_FAILURE              0x2023
#define PMERR_DOSREAD_FAILURE              0x2024
#define PMERR_DRIVER_NOT_FOUND             0x2025
#define PMERR_DUP_SEG                      0x2026
#define PMERR_DYNAMIC_SEG_SEQ_ERROR        0x2027
#define PMERR_DYNAMIC_SEG_ZERO_INV         0x2028
#define PMERR_ELEMENT_INCOMPLETE           0x2029
#define PMERR_ESC_CODE_NOT_SUPPORTED       0x202A
#define PMERR_EXCEEDS_MAX_SEG_LENGTH       0x202B
#define PMERR_FONT_AND_MODE_MISMATCH       0x202C
#define PMERR_FONT_FILE_NOT_LOADED         0x202D
#define PMERR_FONT_NOT_LOADED              0x202E
#define PMERR_FONT_TOO_BIG                 0x202F
#define PMERR_ID_HAS_NO_BITMAP             0x2030
#define PMERR_IMAGE_INCOMPLETE             0x2031
#define PMERR_INCOMPAT_COLOR_FORMAT        0x2032
#define PMERR_INCOMPAT_COLOR_OPTIONS       0x2033
#define PMERR_INCOMPATIBLE_BITMAP          0x2034
#define PMERR_INCOMPATIBLE_METAFILE        0x2035
#define PMERR_INCORRECT_DC_TYPE            0x2036
#define PMERR_INSUFFICIENT_DISK_SPACE      0x2037
#define PMERR_INSUFFICIENT_MEMORY          0x2038
#define PMERR_INV_ANGLE_PARM               0x2039
#define PMERR_INV_ARC_CONTROL              0x203A
#define PMERR_INV_AREA_CONTROL             0x203B
#define PMERR_INV_ATTR_MODE                0x203C
#define PMERR_INV_BACKGROUND_COL_ATTR      0x203D
#define PMERR_INV_BACKGROUND_MIX_ATTR      0x203E
#define PMERR_INV_BITBLT_MIX               0x203F
#define PMERR_INV_BITBLT_STYLE             0x2040
#define PMERR_INV_BITMAP_DIMENSION         0x2041
#define PMERR_INV_BOX_CONTROL              0x2042
#define PMERR_INV_BOX_ROUNDING_PARM        0x2043
#define PMERR_INV_CHAR_ANGLE_ATTR          0x2044
#define PMERR_INV_CHAR_DIRECTION_ATTR      0x2045
#define PMERR_INV_CHAR_MODE_ATTR           0x2046
#define PMERR_INV_CHAR_POS_OPTIONS         0x2047
#define PMERR_INV_CHAR_SET_ATTR            0x2048
#define PMERR_INV_CHAR_SHEAR_ATTR          0x2049
#define PMERR_INV_CLIP_PATH_OPTIONS        0x204A
#define PMERR_INV_CODEPAGE                 0x204B
#define PMERR_INV_COLOR_ATTR               0x204C
#define PMERR_INV_COLOR_DATA               0x204D
#define PMERR_INV_COLOR_FORMAT             0x204E
#define PMERR_INV_COLOR_INDEX              0x204F
#define PMERR_INV_COLOR_OPTIONS            0x2050
#define PMERR_INV_COLOR_START_INDEX        0x2051
#define PMERR_INV_COORD_OFFSET             0x2052
#define PMERR_INV_COORD_SPACE              0x2053
#define PMERR_INV_COORDINATE               0x2054
#define PMERR_INV_CORRELATE_DEPTH          0x2055
#define PMERR_INV_CORRELATE_TYPE           0x2056
#define PMERR_INV_DC_DATA                  0x2057
#define PMERR_INV_DC_TYPE                  0x2058
#define PMERR_INV_DEVICE_NAME              0x2059
#define PMERR_INV_DRAW_CONTROL             0x205A
#define PMERR_INV_DRAW_VALUE               0x205B
#define PMERR_INV_DRAWING_MODE             0x205C
#define PMERR_INV_DRIVER_DATA              0x205D
#define PMERR_INV_EDIT_MODE                0x205E
#define PMERR_INV_ELEMENT_OFFSET           0x205F
#define PMERR_INV_ELEMENT_POINTER          0x2060
#define PMERR_INV_ESC_CODE                 0x2061
#define PMERR_INV_ESCAPE_DATA              0x2062
#define PMERR_INV_FILL_PATH_OPTIONS        0x2063
#define PMERR_INV_FIRST_CHAR               0x2064
#define PMERR_INV_FONT_ATTRS               0x2065
#define PMERR_INV_FONT_FILE_DATA           0x2066
#define PMERR_INV_FOR_THIS_DC_TYPE         0x2067
#define PMERR_INV_FORMAT_CONTROL           0x2068
#define PMERR_INV_FORMS_CODE               0x2069
#define PMERR_INV_GEOM_LINE_WIDTH_ATTR     0x206A
#define PMERR_INV_GETDATA_CONTROL          0x206B
#define PMERR_INV_GRAPHICS_FIELD           0x206C
#define PMERR_INV_HBITMAP                  0x206D
#define PMERR_INV_HDC                      0x206E
#define PMERR_INV_HMF                      0x206F
#define PMERR_INV_HPS                      0x2070
#define PMERR_INV_HRGN                     0x2071
#define PMERR_INV_ID                       0x2072
#define PMERR_INV_IMAGE_DATA_LENGTH        0x2073
#define PMERR_INV_IMAGE_DIMENSION          0x2074
#define PMERR_INV_IMAGE_FORMAT             0x2075
#define PMERR_INV_IN_AREA                  0x2076
#define PMERR_INV_IN_CURRENT_EDIT_MODE     0x2077
#define PMERR_INV_IN_DRAW_MODE             0x2078
#define PMERR_INV_IN_ELEMENT               0x2079
#define PMERR_INV_IN_IMAGE                 0x207A
#define PMERR_INV_IN_PATH                  0x207B
#define PMERR_INV_IN_RETAIN_MODE           0x207C
#define PMERR_INV_IN_SEG                   0x207D
#define PMERR_INV_IN_VECTOR_SYMBOL         0x207E
#define PMERR_INV_INFO_TABLE               0x207F
#define PMERR_INV_KERNING_FLAGS            0x2080
#define PMERR_INV_LENGTH_OR_COUNT          0x2081
#define PMERR_INV_LINE_END_ATTR            0x2082
#define PMERR_INV_LINE_JOIN_ATTR           0x2083
#define PMERR_INV_LINE_TYPE_ATTR           0x2084
#define PMERR_INV_LINE_WIDTH_ATTR          0x2085
#define PMERR_INV_LOGICAL_ADDRESS          0x2086
#define PMERR_INV_MARKER_BOX_ATTR          0x2087
#define PMERR_INV_MARKER_SET_ATTR          0x2088
#define PMERR_INV_MARKER_SYMBOL_ATTR       0x2089
#define PMERR_INV_MATRIX_ELEMENT           0x208A
#define PMERR_INV_MAX_HITS                 0x208B
#define PMERR_INV_METAFILE                 0x208C
#define PMERR_INV_METAFILE_LENGTH          0x208D
#define PMERR_INV_METAFILE_OFFSET          0x208E
#define PMERR_INV_MICROPS_DRAW_CONTROL     0x208F
#define PMERR_INV_MICROPS_FUNCTION         0x2090
#define PMERR_INV_MICROPS_ORDER            0x2091
#define PMERR_INV_MIX_ATTR                 0x2092
#define PMERR_INV_MODE_FOR_OPEN_DYN        0x2093
#define PMERR_INV_MODE_FOR_REOPEN_SEG      0x2094
#define PMERR_INV_MODIFY_PATH_MODE         0x2095
#define PMERR_INV_MULTIPLIER               0x2096
#define PMERR_INV_OR_INCOMPAT_OPTIONS      0x2097
#define PMERR_INV_ORDER_LENGTH             0x2098
#define PMERR_INV_ORDERING_PARM            0x2099
#define PMERR_INV_OUTSIDE_DRAW_MODE        0x209A
#define PMERR_INV_PAGE_VIEWPORT            0x209B
#define PMERR_INV_PATH_ID                  0x209C
#define PMERR_INV_PATH_MODE                0x209D
#define PMERR_INV_PATTERN_ATTR             0x209E
#define PMERR_INV_PATTERN_REF_PT_ATTR      0x209F
#define PMERR_INV_PATTERN_SET_ATTR         0x20A0
#define PMERR_INV_PATTERN_SET_FONT         0x20A1
#define PMERR_INV_PICK_APERTURE_OPTION     0x20A2
#define PMERR_INV_PICK_APERTURE_POSN       0x20A3
#define PMERR_INV_PICK_APERTURE_SIZE       0x20A4
#define PMERR_INV_PICK_NUMBER              0x20A5
#define PMERR_INV_PLAY_METAFILE_OPTION     0x20A6
#define PMERR_INV_PRIMITIVE_TYPE           0x20A7
#define PMERR_INV_PS_SIZE                  0x20A8
#define PMERR_INV_PUTDATA_FORMAT           0x20A9
#define PMERR_INV_QUERY_ELEMENT_NO         0x20AA
#define PMERR_INV_RECT                     0x20AB
#define PMERR_INV_REGION_CONTROL           0x20AC
#define PMERR_INV_REGION_MIX_MODE          0x20AD
#define PMERR_INV_REPLACE_MODE_FUNC        0x20AE
#define PMERR_INV_RESERVED_FIELD           0x20AF
#define PMERR_INV_RESET_OPTIONS            0x20B0
#define PMERR_INV_RGBCOLOR                 0x20B1
#define PMERR_INV_SCAN_START               0x20B2
#define PMERR_INV_SEG_ATTR                 0x20B3
#define PMERR_INV_SEG_ATTR_VALUE           0x20B4
#define PMERR_INV_SEG_CH_LENGTH            0x20B5
#define PMERR_INV_SEG_NAME                 0x20B6
#define PMERR_INV_SEG_OFFSET               0x20B7
#define PMERR_INV_SETID                    0x20B8
#define PMERR_INV_SHARPNESS_PARM           0x20B9
#define PMERR_INV_SOURCE_OFFSET            0x20BA
#define PMERR_INV_STOP_DRAW_VALUE          0x20BB
#define PMERR_INV_TRANSFORM_TYPE           0x20BC
#define PMERR_INV_USAGE_PARM               0x20BD
#define PMERR_INV_VIEWING_LIMITS           0x20BE
#define PMERR_KERNING_NOT_SUPPORTED        0x20BF
#define PMERR_LABEL_NOT_FOUND              0x20C0
#define PMERR_MATRIX_OVERFLOW              0x20C1
#define PMERR_METAFILE_IN_USE              0x20C2
#define PMERR_METAFILE_LIMIT_EXCEEDED      0x20C3
#define PMERR_NAME_STACK_FULL              0x20C4
#define PMERR_NO_BITMAP_SELECTED           0x20C5
#define PMERR_NO_CURRENT_ELEMENT           0x20C6
#define PMERR_NO_CURRENT_SEG               0x20C7
#define PMERR_NO_METAFILE_RECORD_HANDLE    0x20C8
#define PMERR_NOT_CREATED_BY_DEVOPENDC     0x20C9
#define PMERR_NOT_IN_AREA                  0x20CA
#define PMERR_NOT_IN_DRAW_MODE             0x20CB
#define PMERR_NOT_IN_ELEMENT               0x20CC
#define PMERR_NOT_IN_IMAGE                 0x20CD
#define PMERR_NOT_IN_PATH                  0x20CE
#define PMERR_NOT_IN_RETAIN_MODE           0x20CF
#define PMERR_NOT_IN_SEG                   0x20D0
#define PMERR_ORDER_TOO_BIG                0x20D1
#define PMERR_OTHER_SET_ID_REFS            0x20D2
#define PMERR_OVERRAN_SEG                  0x20D3
#define PMERR_OWN_SET_ID_REFS              0x20D4
#define PMERR_PATH_INCOMPLETE              0x20D5
#define PMERR_PATH_LIMIT_EXCEEDED          0x20D6
#define PMERR_PATH_UNKNOWN                 0x20D7
#define PMERR_PEL_IS_CLIPPED               0x20D8
#define PMERR_PEL_NOT_AVAILABLE            0x20D9
#define PMERR_PRIMITIVE_STACK_EMPTY        0x20DA
#define PMERR_PROLOG_ERROR                 0x20DB
#define PMERR_PROLOG_SEG_ATTR_NOT_SET      0x20DC
#define PMERR_PS_BUSY                      0x20DD
#define PMERR_PS_IS_ASSOCIATED             0x20DE
#define PMERR_REALIZE_NOT_SUPPORTED        0x20DF
#define PMERR_REGION_IS_CLIP_REGION        0x20E0
#define PMERR_RESOURCE_DEPLETION           0x20E1
#define PMERR_SEG_AND_REFSEG_ARE_SAME      0x20E2
#define PMERR_SEG_CALL_RECURSIVE           0x20E3
#define PMERR_SEG_CALL_STACK_EMPTY         0x20E4
#define PMERR_SEG_CALL_STACK_FULL          0x20E5
#define PMERR_SEG_IS_CURRENT               0x20E6
#define PMERR_SEG_NOT_CHAINED              0x20E7
#define PMERR_SEG_NOT_FOUND                0x20E8
#define PMERR_SEG_STORE_LIMIT_EXCEEDED     0x20E9
#define PMERR_SETID_IN_USE                 0x20EA
#define PMERR_SETID_NOT_FOUND              0x20EB
#define PMERR_STARTDOC_NOT_ISSUED          0x20EC
#define PMERR_STOP_DRAW_OCCURRED           0x20ED
#define PMERR_TOO_MANY_METAFILES_IN_USE    0x20EE
#define PMERR_TRUNCATED_ORDER              0x20EF
#define PMERR_UNCHAINED_SEG_ZERO_INV       0x20F0
#define PMERR_UNSUPPORTED_ATTR             0x20F1
#define PMERR_UNSUPPORTED_ATTR_VALUE       0x20F2
 
#endif /* non-common GPIERRORS */
