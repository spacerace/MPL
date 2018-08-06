/*
    paint.c -- Paints the clock

    Created by Microsoft Corporation, 1989
*/
#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include "clock.h"
#include "dialogs.h"

#define MATLF_SIZE ( sizeof ( MATRIXLF ) / sizeof ( LONG ) )

#define CLK_MAJORTICKS	0x0001
#define CLK_MINORTICKS	0x0002

extern BOOL fIconic , fShowSecondHand ;
extern DATETIME dt ;
extern HPS hps ;

VOID ClkPaint ( HWND ) ;
VOID ClkDrawHand ( HPS , SHORT , SHORT ) ;
VOID ClkDrawFace ( HPS ) ;
VOID ClkDrawTicks ( HPS , USHORT ) ;

static FIXED fxSin [60] = {
    0x00000000, 0x00001ac2, 0x00003539, 0x00004f1b, 0x0000681f, 0x00007fff,
    0x00009679, 0x0000ab4c, 0x0000be3e, 0x0000cf1b, 0x0000ddb3, 0x0000e9de,
    0x0000f378, 0x0000fa67, 0x0000fe98, 0x0000ffff, 0x0000fe98, 0x0000fa67,
    0x0000f378, 0x0000e9de, 0x0000ddb3, 0x0000cf1b, 0x0000be3e, 0x0000ab4c,
    0x00009679, 0x00008000, 0x00006820, 0x00004f1b, 0x00003539, 0x00001ac2,
    0x00000000, 0xffffe53e, 0xffffcac7, 0xffffb0e5, 0xffff97e1, 0xffff8001,
    0xffff6988, 0xffff54b5, 0xffff41c2, 0xffff30e5, 0xffff224d, 0xffff1622,
    0xffff0c88, 0xffff0599, 0xffff0168, 0xffff0001, 0xffff0167, 0xffff0599,
    0xffff0c88, 0xffff1622, 0xffff224d, 0xffff30e5, 0xffff41c2, 0xffff54b4,
    0xffff6987, 0xffff8000, 0xffff97e0, 0xffffb0e4, 0xffffcac6, 0xffffe53e
} ;

USHORT usMajorTickPref = CLKTM_ALWAYS ;
USHORT usMinorTickPref = CLKTM_NOTICONIC ;
LONG clrBackground = CLR_DARKCYAN ;
LONG clrFace = CLR_CYAN ;
LONG clrHourHand = CLR_PINK ;
LONG clrMinuteHand = CLR_DARKPINK ;


/*
    ClkPaint()  -- Paints the clock client window
*/
VOID ClkPaint ( HWND hwnd )
{
    RECTL rclUpdate ;

    /* fill the invalid rectangle */
    WinBeginPaint ( hwnd , hps , & rclUpdate ) ;
    WinFillRect ( hps , & rclUpdate , clrBackground ) ;

    /* draw the face, the hour hand, and the minute hand */
    ClkDrawFace ( hps ) ;
    ClkDrawHand ( hps , HT_HOUR , dt . hours ) ;
    ClkDrawHand ( hps , HT_MINUTE , dt . minutes ) ;

    /* draw the tick marks */
    if ( ( usMajorTickPref == CLKTM_ALWAYS ) ||
	 ( ( usMajorTickPref == CLKTM_NOTICONIC ) && ! fIconic ) )
	ClkDrawTicks ( hps , CLK_MAJORTICKS ) ;

    if ( ( usMinorTickPref == CLKTM_ALWAYS ) ||
	 ( ( usMinorTickPref == CLKTM_NOTICONIC ) && ! fIconic ) )
	ClkDrawTicks ( hps , CLK_MINORTICKS ) ;

    /* draw the second hand last, so xor will work */
    if ( fShowSecondHand )
	ClkDrawHand ( hps , HT_SECOND , dt . seconds ) ;

    /* restore the presentation space */
    WinEndPaint ( hps ) ;
}


/*
    ClkDrawHand() -- Draws specified hand at specified hour in given PS
*/
VOID ClkDrawHand ( HPS hps , SHORT sHandType , SHORT sAngle )
{
    static POINTL aptlHour [ ] = { { 8 , 0 } , { 0 , 60 } , { -8 , 0 } ,
				   { 0 , -10 } , { 8 , 0 } } ;
    static POINTL aptlMinute [ ] = { { 6 , 0 } , { 0 , 80 } , { -6 , 0 } ,
				     { 0 , -15 } , { 6 , 0 } } ;
    static POINTL aptlSecond [ ] = { { 0 , -15 } , { 0 , 85 } } ;
    static LONG cptlHour = sizeof ( aptlHour ) / sizeof ( POINTL ) ;
    static LONG cptlMinute = sizeof ( aptlMinute ) / sizeof ( POINTL ) ;
    static LONG cptlSecond = sizeof ( aptlSecond ) / sizeof ( POINTL ) ;
    BOOL f ;
    static MATRIXLF matlfModel = {
	MAKEFIXED ( 1 , 0 ) ,	    MAKEFIXED ( 0 , 0 ) ,	0L ,
	MAKEFIXED ( 0 , 0 ) ,	    MAKEFIXED ( 1 , 0 ) ,	0L ,
	100L ,			    100L ,			1L } ;

    /* prepare a rotation transform and set it into the ps */
    /*	    cos x    - sin x	0     *\
    |	    sin x      cos x	0      |
    \*	    100        100	1     */

    matlfModel . fxM11 =
    matlfModel . fxM22 = fxSin [ ( sAngle + 15 ) % 60 ] ;
    matlfModel . fxM12 = fxSin [ ( sAngle + 30 ) % 60 ] ;
    matlfModel . fxM21 = fxSin [ sAngle ] ;
    f = GpiSetModelTransformMatrix ( hps , ( LONG ) MATLF_SIZE ,
				     & matlfModel , TRANSFORM_REPLACE ) ;

    /* draw the specified hand */

    switch ( sHandType ) {

	case HT_HOUR:
	    GpiSetColor ( hps , clrHourHand ) ;
	    GpiBeginPath ( hps , 1L ) ;
	    GpiSetCurrentPosition ( hps , aptlHour ) ;
	    GpiPolyLine ( hps , cptlHour , aptlHour ) ;
	    GpiEndPath ( hps ) ;
	    GpiFillPath ( hps , 1L , FPATH_ALTERNATE ) ;
	    break;

	case HT_MINUTE:
	    GpiSetColor ( hps , clrMinuteHand ) ;
	    GpiBeginPath ( hps , 1L ) ;
	    GpiSetCurrentPosition ( hps , aptlMinute ) ;
	    GpiPolyLine ( hps , cptlMinute , aptlMinute ) ;
	    GpiEndPath ( hps ) ;
	    GpiFillPath ( hps , 1L , FPATH_ALTERNATE ) ;
	    break;

	case HT_SECOND:
	    /* draw in XOR mixmode, so we can undraw later */
	    GpiSetMix ( hps , FM_INVERT ) ;
	    GpiSetCurrentPosition ( hps , aptlSecond ) ;
	    GpiPolyLine ( hps , cptlSecond , aptlSecond ) ;
	    GpiSetMix ( hps , FM_OVERPAINT ) ;
	    break;
    }
}


/*
    ClkDrawFace() -- Draws clock face and tick marks
*/
VOID ClkDrawFace ( HPS hps )
{
    BOOL f ;
    LONG l ;
    static POINTL ptlOrigin = { 0L , 0L } ;

    /*	      1 	0      0     *\
    *	      0 	1      0      *
    \*	    100       100      1     */

    static MATRIXLF matlfModel = {
	MAKEFIXED ( 1 , 0 ) ,	    MAKEFIXED ( 0 , 0 ) ,	0L ,
	MAKEFIXED ( 0 , 0 ) ,	    MAKEFIXED ( 1 , 0 ) ,	0L ,
	100L ,			    100L ,			1L } ;

    /* center at (100, 100) and draw the clock face */
    f = GpiSetModelTransformMatrix ( hps , ( LONG ) MATLF_SIZE ,
				     & matlfModel , TRANSFORM_REPLACE ) ;
    GpiSetColor ( hps , clrFace ) ;
    GpiSetCurrentPosition ( hps , & ptlOrigin ) ;
    l = GpiFullArc ( hps , DRO_OUTLINEFILL , MAKEFIXED ( 85 , 0 ) ) ;
}


/*
    ClkDrawTicks() -- Draws clock ticks
*/
VOID ClkDrawTicks ( HPS hps , USHORT usTicks )
{
    BOOL f ;
    USHORT usAngle ;

    /* prepare a transform to use when rotating the ticks */
    /*	    cos x    - sin x	0     *\
    |	    sin x      cos x	0      |
    \*	    100        100	1     */

    static MATRIXLF matlfModel = {
	MAKEFIXED ( 1 , 0 ) ,	    MAKEFIXED ( 0 , 0 ) ,	0L ,
	MAKEFIXED ( 0 , 0 ) ,	    MAKEFIXED ( 1 , 0 ) ,	0L ,
	100L ,			    100L ,			1L } ;

    /* define what the tick marks look like */
    static POINTL aptlMajorTick [ ] = { { -3 , 94 } , { 3 , 100 } } ;
    static POINTL aptlMinorTick [ ] = { { 0 , 94 } , { 0 , 95 } } ;

    /* draw in the default color */
    GpiSetColor ( hps , CLR_DEFAULT ) ;

    /* have we been asked to draw the major ticks? */
    if ( usTicks & CLK_MAJORTICKS )
	for ( usAngle = 0 ; usAngle < 60 ; usAngle += 5 ) {

	    /* set the rotation transform */
	    matlfModel . fxM11 =
	    matlfModel . fxM22 = fxSin [ ( usAngle + 15 ) % 60 ] ;
	    matlfModel . fxM12 = fxSin [ ( usAngle + 30 ) % 60 ] ;
	    matlfModel . fxM21 = fxSin [ usAngle ] ;
	    f = GpiSetModelTransformMatrix ( hps , ( LONG ) MATLF_SIZE ,
					     & matlfModel ,
					     TRANSFORM_REPLACE ) ;

	    /* draw a major tick mark */
	    GpiSetCurrentPosition ( hps , & aptlMajorTick [ 0 ] ) ;
	    GpiBox ( hps , DRO_FILL , & aptlMajorTick [ 1 ] , 0L , 0L ) ;
    }

    /* have we been asked to draw the minor ticks? */
    if ( usTicks & CLK_MINORTICKS )
	for ( usAngle = 0 ; usAngle < 60 ; usAngle ++ ) {

	    matlfModel . fxM11 =
	    matlfModel . fxM22 = fxSin [ ( usAngle + 15 ) % 60 ] ;
	    matlfModel . fxM12 = fxSin [ ( usAngle + 30 ) % 60 ] ;
	    matlfModel . fxM21 = fxSin [ usAngle ] ;

	    /* set the transform */
	    f = GpiSetModelTransformMatrix ( hps , ( LONG ) MATLF_SIZE ,
					     & matlfModel ,
					     TRANSFORM_REPLACE ) ;

	    /* draw a minor tick mark */
	    GpiSetCurrentPosition ( hps , & aptlMinorTick [ 0 ] ) ;
	    GpiLine ( hps , & aptlMinorTick [ 1 ] ) ;
    }
}
