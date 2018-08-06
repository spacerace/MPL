/* mouse.c   - Mouse interface routines for DeSmet C.

	       A library of the functions available on the MICROSOFT
	       mouse via interrupt #51.  These are the functions 0-15
	       which are described in the MS mouse documentation on pages
	       49-85.  Each function has been defined so that it can be
	       called directly from the DeSmet "C" compiler.  The code
	       itself is not difficult to understand.  In fact, each call
	       is nothing more than an assignment of the proper values
	       to the 8088 registers and subsequent execution of DOS
	       interrupt #51.  The DeSmet compiler uses a function called
	       _doint() to accomplish the interrupt request.  The original mouse interface routines used the Lattice int86() function do perform an interrupt, so a marco was used to change the call to the proper DeSmet function.  This function
	       makes use of global variables _rax, _rbx, etc to represent
	       the microprocessor registers.  Adapting this code to other
	       compilers should thus be a relatively easy task since all
	       that needs to be done is to replace the calls to int86()
	       with sysint() or whatever your compiler calls the routine.

		  In order to guarantee that outside subroutines would
	       have access to the results of these functions, and to make
	       function calls as simple as possible, I had to make use of
	       a large number of external variables.  Their names are
	       listed here so that they will not conflict with any of the
	       variables within the scope of the calling functions:

	       TYPE  -	  NAME	-     PURPOSE
	       ************************************************************
	       int	  mse_xin	x-input coordinate
	       int	  mse_yin	y-input coordinate
	       int	  prscnt[2]	array for count of button presses
	       int	  x_lstprs[2]	array of x-coords at last press
	       int	  y_lstprs[2]	array of y-coords at last press
	       int	  rlscnt[2]	array for count of button releases
	       int	  x_lstrls[2]	array of x-coords at last release
	       int	  y_lstrls[2]	array of y-coords at last release
	       int	  h_micks	horizontal motion counter
	       int	  v_micks	vertical motion counters


		  That's it for external variables.  You will need to use
	       these names in your own programs to get the results of the
	       various function calls.	I have tried to document each of
	       the functions well enough that the user will not be confused
	       about which external variables are needed.

		  The following is a list of the functions included at this
	       point:
 m_installed()	       - Returns 1 if a mouse is installed, NULL if none.
			    NOTE:  This routine is NOT portable.  It is
			    very sensitive to the operating environment.
			    Please let me know if you come up with a better
			    way of checking for the mouse driver software.
 m_reset()	       - Reset mouse driver and return current status
 m_showcur()	       - Increment cursor flag and display cursor
 m_hidecur()	       - Decrement cursor flag and hide cursor
 m_readloc()	       - Read cursor position and status
 m_setloc()	       - Set cursor position
 m_keyprs(button)      - Get mouse button press information
 m_keyrls(button)      - Get mouse button release information
 m_setxlim(xmin, xmax) - Set horizontal minimum and maximum
 m_setylim(ymin, ymax) - Set vertical minimum and maximum
 m_defgcur(hot_x, hot_y, curshape)     - Define graphics cursor
 m_deftcur(cur_type, scr_msk, cur_msk) - Define text cursor
 m_mickeys()	       - Read mouse motion counters
 m_inpmsk()	       - Set user-defined subroutine call mask
 m_penon()	       - Turn light-pen emulation on
 m_penoff()	       - Turn light-pen emulation off
 m_setasp()	       - Set horizontal and vertical motion sensitivity

The original Lattice functions were modified on 3-2-85 for use with DeSmet C.

 On to the functions! */

/*************************************************************************/

#define LEFT  0
#define RIGHT 1
#define int86(x)	    _doint(x)

int mse_xin, mse_yin;
int prscnt[2], x_lstprs[2], y_lstprs[2];
int rlscnt[2], x_lstrls[2], y_lstrls[2];
int h_micks, v_micks;

extern int _rax, _rbx, _rcx, _rdx;

m_installed()	    /* see if mouse driver software is installed */
{
    /*	This is a TERRIBLE way to check for the driver software.  Please */
    /*	send FidoMail to Jeff Porter on Fido 103/1985, 103/50, or 103/91 */
    /*	if you discover a better way of checking.  Thanks! */

    int seg, off;

    off = _peek(51*4, 0);	/* low byte of int. vector offset */
    off += _peek(51*4+1, 0)<<8; /* high byte of int. vector offset */
    seg = _peek(51*4+2, 0);	/* low byte of int. vector segment */
    seg += _peek(51*4+3, 0)<<8; /* high byte of int. vector segment */

    if ( off == 0 && seg == 0 ) /* vector is not set */
	return(0);
    if ( _peek( off, seg ) != 0xeb )	/* check byte of mouse driver */
	return(0);
    if ( _peek( off+1, seg ) != 0x64 )	/* next byte */ /* modified 7-1-85 */
	return(0);
    if ( _peek( off+2, seg ) != 0x55 )	/* and next */
	return(0);
    if ( _peek( off+3, seg ) != 0x8b )	/* and the next */
	return(0);
    return(1);				/* four bytes ok, assume it is loaded */
}

/***************************************************************/

m_reset()     /* reset mouse driver and return status of mouse */
{
   _rax = 0;		    /* set mouse function call 0 */
   int86( 51 ); 	    /* execute mouse interrupt */
   return(_rbx);	    /* return status code */
}

/****************************************************************/

m_showcur()    /* increment internal cursor flag and display cursor */
{

   _rax = 1;		    /* set mouse function call 1 */
   int86( 51 ); 	    /* execute mouse interrupt */
   return(0);		    /* no output from function */
}


/****************************************************************/

m_hidecur()    /* decrement internal cursor flag and hide cursor */
{

   _rax = 2;		    /* set mouse function call 2 */
   int86( 51 ); 	    /* execute mouse interrupt */
   return(0);		    /* no output from function */
}


/****************************************************************/

m_readloc()    /* get cursor position and button status */
{
   extern int mse_xin;	  /* global integer x-coordinate */
   extern int mse_yin;	  /* global integer y-coordinate */

   _rax = 3;		    /* set mouse function call 3 */
   int86( 51 ); 	    /* execute mouse interrupt */
   mse_xin = _rcx;	    /* store new x-coordinate */
   mse_yin = _rdx;	    /* store new y-coordinate */
   return(_rbx);	    /* return button status */
}

/****************************************************************/

m_setloc(x, y)	  /* set mouse cursor location */
int x;		  /* integer x-coordinate parameter */
int y;		  /* integer y-coordinate parameter */
{

   _rax = 4;		      /* set mouse function call 4 */
   _rcx = x;		      /* assign new x-coordinate */
   _rdx = y;		      /* assign new y-coordinate */
   int86( 51 ); 	      /* execute mouse interrupt */
   return(0);		      /* no function output */
}

/******************************************************************/

m_keyprs(button)    /* get button press information */
int button;	    /* parameter for left or right button */
{
   extern int prscnt[]; 	 /* array for count of button presses */
   extern int x_lstprs[];	 /* array for x-coordinate at last press */
   extern int y_lstprs[];	 /* array for y-coordinate at last press */

   _rax = 5;		/* set mouse function call 5 */
   _rbx = button;	/* pass parameter for left or right button */
   int86( 51 ); 	/* execute mouse interrupt */
   prscnt[button] = _rbx;    /* store updated press count */
   x_lstprs[button] = _rcx;  /* x-coordinate at last press */
   y_lstprs[button] = _rdx;  /* y-coordinate at last press */
   return(_rax);	     /* return button status */
}

/******************************************************************/

m_keyrls(button)    /* get button release information */
int button;	    /* parameter for left or right button */
{
   extern int rlscnt[]; 	 /* array for count of button releases */
   extern int x_lstrls[];	 /* array for x-coordinate at last release */
   extern int y_lstrls[];	 /* array for y-coordinate at last release */

   _rax = 6;		/* set mouse function call 6 */
   _rbx = button;	/* pass parameter for left or right button */
   int86( 51 ); 	/* execute mouse interrupt */
   rlscnt[button] = _rbx;    /* store updated release count */
   x_lstrls[button] = _rcx;  /* x-coordinate at last release */
   y_lstrls[button] = _rdx;  /* y-coordinate at last release */
   return(_rax);	     /* return button status */
}

/******************************************************************/

m_setxlim( xmin, xmax)	/* set horizontal minimum and maximum coordinates */
int xmin;		/* minimum x-coordinate for mouse activity */
int xmax;		/* maximum x-coordinate for mouse activity */
{

   _rax = 7;		/* set mouse function call 7 */
   _rcx = xmin; 	/* load horizontal minimum parameter */
   _rdx = xmax; 	/* load horizontal maximum parameter */
   int86( 51 ); 	/* execute mouse interrupt */
   return(0);		/* no function output */
}

/******************************************************************/

m_setylim( ymin, ymax)	/* set vertical minimum and maximum coordinates */
int ymin;		/* minimum y-coordinate for mouse activity */
int ymax;		/* maximum y-coordinate for mouse activity */
{

   _rax = 8;		/* set mouse function call 8 */
   _rcx = ymin; 	/* load vertical minimum parameter */
   _rdx = ymax; 	/* load vertical maximum parameter */
   int86( 51 ); 	/* execute mouse interrupt */
   return(0);		/* no function output */
}

/******************************************************************/

m_defgcur( hot_x, hot_y, cur_shape)   /* define graphics cursor attributes */
int hot_x;		     /* cursor field x-coordinate hot-spot */
int hot_y;		     /* cursor field y-coordinate hot-spot */
int cur_shape;		     /* pointer to array of cursor and screen masks */
{

   _rax = 9;		/* set mouse function call 9 */
   _rbx = hot_x;	/* load horizontal hot-spot */
   _rcx = hot_y;	/* load vertical hot-spot */
   _rdx = cur_shape;	/* load cursor array pointer address */
   int86( 51 ); 	/* execute mouse interrupt */
   return(0);		/* no function output */
}

/******************************************************************/

m_deftcur( cur_type, scr_msk, cur_msk)	/* define text cursor type and masks */
int cur_type;		      /* parameter for software or hardware cursor */
int scr_msk;		      /* screen mask value - scan line start */
int cur_msk;		      /* cursor mask value - scan line stop  */
{

   _rax = 10;		/* set mouse function call 10 */
   _rbx = cur_type;	/* load cursor type parameter */
   _rcx = scr_msk;	/* load screen mask value */
   _rdx = cur_msk;	/* load cursor mask value */
   int86( 51 ); 	/* execute mouse interrupt */
   return(0);		/* no function output */
}

/******************************************************************/

m_mickeys()			 /* read mouse motion counters */
{
   extern int h_micks;		 /* horizontal motion units accumulator */
   extern int v_micks;		 /* vertical motion units accumulator */

   _rax = 11;		/* set mouse function call 11 */
   int86( 51 ); 	/* execute mouse interrupt */
   h_micks = _rcx;	/* store horizontal motion units */
   v_micks = _rdx;	/* store vertical motion units */
   return(0);		/* no function output */
}

/******************************************************************/

m_inpmsk( call_msk, offset)	 /* set user defined subroutine and mask */
int call_msk;			 /* call mask - defines interrupt condition */
int offset;			 /* address offset to user subroutine */
{

   _rax = 12;		/* set mouse function call 12 */
   _rcx = call_msk;	/* load call mask value */
   _rdx = offset;	/* load user subroutine address */
   int86( 51 ); 	/* execute mouse interrupt */
   return(0);		/* no function output */
}

/*****************************************************************/

m_penon()		/* turn light-pen emulation on */
{

   _rax = 13;		/* set mouse function call 13 */
   int86( 51 ); 	/* execute mouse interrupt */
   return(0);		/* no function output */
}

/*****************************************************************/

m_penoff()			 /* turn light-pen emulation off */
{

   _rax = 14;		/* set mouse function call 14 */
   int86( 51 ); 	/* execute mouse interrupt */
   return(0);		/* no function output */
}

/*****************************************************************/

m_setasp( h_ratio, v_ratio)	 /* set cursor motion counter sensitivity */
int h_ratio;			 /* horizontal mickey:pixel ratio */
int v_ratio;			 /* vertical mickey:pixel ratio */
{

   _rax = 15;		/* set mouse function call 15 */
   _rcx = h_ratio;	/* load horizontal ratio value */
   _rdx = v_ratio;	/* load vertical ratio value */
   int86( 51 ); 	/* execute mouse interrupt */
   return(0);		/* no function output */
}

