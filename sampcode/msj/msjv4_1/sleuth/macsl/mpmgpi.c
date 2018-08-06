/*-----------------------------------------------------------------*/
/* MpmGPI.c                                                        */
/* GPI functions                                                   */
/*-----------------------------------------------------------------*/

#include "MacPM.h"

/*-----------------------------------------------------------------*/
/* Draw a character string at the current position                 */
/*-----------------------------------------------------------------*/

LONG APIENTRY GpiCharString( hps, lLen, pch )
    HPS         hps;
    LONG        lLen;
    PCH         pch;
{
    if( ! MpmValidatePS(hps) )
      return GPI_ERROR;

    thePort = PGRAFOFHPS(hps);

    DrawText( pch, 0, (SHORT)lLen );

    return GPI_OK;
}

/*-----------------------------------------------------------------*/
/* Draw a character string at the specified position               */
/*-----------------------------------------------------------------*/

LONG APIENTRY GpiCharStringAt( hps, pptl, lLen, pch )
    HPS         hps;
    PPOINTL     pptl;
    LONG        lLen;
    PCH         pch;
{
    if( ! GpiMove( hps, pptl ) )
      return GPI_ERROR;

    return GpiCharString( hps, lLen, pch );
}

/*-----------------------------------------------------------------*/
/* Erase a PS on the screen.                                       */
/*-----------------------------------------------------------------*/

BOOL APIENTRY GpiErase( hps )
    HPS         hps;
{
    if( ! MpmValidatePS(hps) )
      return FALSE;

    thePort = PGRAFOFHPS(hps);

    EraseRgn( thePort->visRgn );

    return TRUE;
}

/*-----------------------------------------------------------------*/
/* Set the drawing position to *pptl.                              */
/*-----------------------------------------------------------------*/

BOOL APIENTRY GpiMove( hps, pptl )
    HPS         hps;
    PPOINTL     pptl;
{
    PMYWND      pwnd;

    if( ! MpmValidatePS(hps) )
      return FALSE;

    thePort = PGRAFOFHPS(hps);
    pwnd = PMYWNDOF(HWNDOFHPS(hps));

    MoveTo( (SHORT)pptl->x, (SHORT)( pwnd->cy - pptl->y ) );

    return TRUE;
}

/*-----------------------------------------------------------------*/
/* Pick up the current font metrics and return it in *pfm.         */
/* This is kind of hokey and only initializes some of the fields   */
/* properly!                                                       */
/*-----------------------------------------------------------------*/

BOOL APIENTRY GpiQueryFontMetrics( hps, lLen, pfm )
    HPS         hps;
    LONG        lLen;
    PFONTMETRICS pfm;
{
    GrafPtr     pgraf;
    FMInput     fmi;
    FMOutPtr    pfmo;

    if( ! MpmValidatePS(hps) )
      return FALSE;

    pgraf = PGRAFOFHPS( hps );

    fmi.family    = pgraf->txFont;
    fmi.size      = pgraf->txSize;
    fmi.face      = pgraf->txFace;
    fmi.needBits  = FALSE;
    fmi.device    = 0;
    fmi.numer.h   = 1;
    fmi.numer.v   = 1;
    fmi.denom.h   = 1;
    fmi.denom.v   = 1;

    pfmo = FMSwapFont( &fmi );
    ASSERT( pfmo,
            "GpiQueryFontMetrics: FMSwapFont failed?" );

    memzero( pfm );

    pfm->lAveCharWidth      =  /* wrong, but make it same as max */
    pfm->lMaxCharInc        = pfmo->widMax;
    pfm->lMaxBaselineExt    = pfmo->ascent + pfmo->descent
                              + pfmo->leading;
    pfm->lMaxDescender      = pfmo->descent;

    /* more later... */

    return TRUE;
}

/*-----------------------------------------------------------------*/
/* Make sure hps is a valid PS handle.                             */
/*-----------------------------------------------------------------*/

LOCAL BOOL MpmValidatePS( hps )
    HPS         hps;
{
    if( hps != _hps1 )
    {
      ERROR( "Invalid PS handle!" );
      return FALSE;
    }

    return TRUE;
}

/*-----------------------------------------------------------------*/
