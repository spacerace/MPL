/************************************************************************
*
*   lfxform.c -- Built-in similarity transforms.
*
*   Created by Microsoft Corporation, 1989
*
************************************************************************/

#define INCL_GPI
#include <os2.h>
#include "linefrac.h"




#define SQRT_2	1.4142136
#define SQRT_3	1.7320508




/************************************************************************
*
*   SharkTooth
*
*   Set up the similarity transform for the following linefractal,
*   which looks roughly like:
*
*
*				 *
*		     ===>      *   *
*			     *	     *
*   ***************	   *	       *
*
************************************************************************/

LINEFRAC SharkTooth[] =
{
    &SharkTooth[1],  PI / 6.0, (1.0/SQRT_3), FALSE,
    EOLIST,	    -PI / 3.0, (1.0/SQRT_3), FALSE
};




/************************************************************************
*
*   SawTooth
*
*   Set up the similarity transform for the following linefractal,
*   which looks roughly like:
*
*
*
*		     ===>      *
*			     *	 *
*   ***************	   *	   *	   *
*				     *	 *
*				       *
*
************************************************************************/

LINEFRAC SawTooth[] =
{
    &SawTooth[1],  PI / 4.0, (0.5 / SQRT_2), FALSE,
    &SawTooth[2], -PI / 2.0, (1.0 / SQRT_2), FALSE,
    EOLIST,	   PI / 2.0, (0.5 / SQRT_2), FALSE
};




/************************************************************************
*
*   KochIsland
*
*   Set up the similarity transform for the following linefractal,
*   which looks roughly like:
*
*   This is known as the Koch, or snowflake, transform.
*
*
*			          *
*		     ===>        * *
*			        *   *
*   ***************	   *****     *****
*
************************************************************************/

LINEFRAC Koch[] =
{
     &Koch[1],		 0.0, 1.0 / 3.0, FALSE,
     &Koch[2],	    PI / 3.0, 1.0 / 3.0, FALSE,
     &Koch[3], -2 * PI / 3.0, 1.0 / 3.0, FALSE,
     EOLIST,	    PI / 3.0, 1.0 / 3.0, FALSE
};




/************************************************************************
*
*   SpaceFiller
*
*   Set up the similarity transform for the following linefractal,
*   which looks roughly like:
*
*
*			   ***B******C***
*			   *		*
*			   A		D
*   **************   ===>  *		*
*
*
*   where sides A and D are reflections.
*
*
*
************************************************************************/

LINEFRAC SpaceFiller[] =
{
    &SpaceFiller[1],  PI / 2.0, 0.5, TRUE,
    &SpaceFiller[2], -PI / 2.0, 0.5, FALSE,
    &SpaceFiller[3],	   0.0, 0.5, FALSE,
    EOLIST,	     -PI / 2.0, 0.5, TRUE
};




/************************************************************************
*
*   StovePipe
*
*   Set up the similarity transform for the following linefractal,
*   which looks roughly like:
*
*   This is sometimes known as the stovepipe transform.
*
*
*			       ******
*			       *    *
*		     ===>      *    *
*			       *    *
*   **************	   *****    *****
*
************************************************************************/

LINEFRAC StovePipe[] =
{
    &StovePipe[1],	 0.0, 1.0 / 3.0, FALSE,
    &StovePipe[2],  PI / 2.0, 1.0 / 3.0, FALSE,
    &StovePipe[3], -PI / 2.0, 1.0 / 3.0, FALSE,
    &StovePipe[4], -PI / 2.0, 1.0 / 3.0, FALSE,
    EOLIST,	    PI / 2.0, 1.0 / 3.0, FALSE
};




/************************************************************************
*
*   SquareWave
*
*   Set up the similarity transform for the following linefractal,
*   which looks roughly like:
*
*
*			   ******
*			   *	*
*   **************   ===>  *	*    *
*				*    *
*				******
*
************************************************************************/

LINEFRAC SquareWave[] =
{
    &SquareWave[1],  PI / 2.0, 0.25, FALSE,
    &SquareWave[2], -PI / 2.0, 0.5,  FALSE,
    &SquareWave[3], -PI / 2.0, 0.5,  FALSE,
    &SquareWave[4],  PI / 2.0, 0.5,  FALSE,
    EOLIST,	     PI / 2.0, 0.25, FALSE
};




/************************************************************************
*
*   HourGlass
*
*   Set up the similarity transform for the following linefractal,
*   which looks roughly like:
*
*
*			      * * * * *
*				*   *
*				  *
*		     ===>	*   *
*			      *       *
*   **************	    *		*
*
************************************************************************/

#define HOURGLASSANGLE	0.463648
#define HOURGLASSLEN	0.83852549

LINEFRAC HourGlass[] =
{
    &HourGlass[1],     HOURGLASSANGLE, HOURGLASSLEN, FALSE,
    &HourGlass[2],  PI-HOURGLASSANGLE, 0.5,	     FALSE,
    EOLIST,	   -PI-HOURGLASSANGLE, HOURGLASSLEN, FALSE
};




/************************************************************************
*
*   aXform
*
*   This lists all the built-in transforms available, with their default
*   coordinate offsets and scale factors.
*
************************************************************************/

XFORMDATA aXform[] =
{
    SharkTooth,  0.200,  0.200,  0.60, 0.60, 3,
    SawTooth,	 0.125,  0.125,  0.75, 0.75, 3,
    Koch,	 0.125,  0.125,  0.75, 0.75, 3,
    SpaceFiller, 0.000, -0.500,  1.00, 1.00, 1,
    StovePipe,	 0.125,  0.125,  0.75, 0.75, 4,
    SquareWave,  0.200,  0.200,  0.60, 0.60, 5,
    HourGlass,	 0.250,  0.250,  0.50, 0.50, 2
};
