/* mouse.c   - Mouse interface routines for Turbo C */

#define LEFT  0
#define RIGHT 1

#include <dos.h>

unsigned m_reset(void);			     /* reset mouse driver and return status of mouse   */
void m_showcur(void);	         /* Increment cursor flag and display cursor        */
void m_hidecur(void);	         /* Decrement cursor flag and hide cursor           */
unsigned m_readloc(void);             /* get cursor position and button status 			*/
void m_setloc(unsigned x, unsigned y);	 /* set mouse cursor location 						*/
unsigned m_keyprs(unsigned button);         /* get button press information 					*/
unsigned m_keyrls(unsigned button);         /* get button release information 				*/
void m_setxlim(unsigned xmin,unsigned xmax);	/* set horizontal minimum and maximum coordinates      */
void m_setylim(unsigned ymin,unsigned ymax);	/* set vertical minimum and maximum coordinates 	   */
void m_defgcur(unsigned hot_x,unsigned hot_y,unsigned cur_shape);    /* define graphics cursor attributes */
void m_deftcur(unsigned cur_type,unsigned scr_msk,unsigned cur_msk); /* define text cursor type and masks */
void m_mickeys(void);	    	 		 /* read mouse motion counters 				*/
void m_inpmsk(unsigned call_msk, unsigned offset); /* set user defined subroutine and mask    */
void m_penon(void);	       		 		 /* Turn light-pen emulation on             */
void m_penoff(void);	       			 /* Turn light-pen emulation off            */
void m_setasp(unsigned h_ratio,unsigned v_ratio);	 /* set cursor motion counter sensitivity   */


unsigned mse_xin, mse_yin;
static unsigned prscnt[3]; 	 	 /* array for count of button presses */
static unsigned x_lstprs[3];	 	 /* array for x-coordinate at last press */
static unsigned y_lstprs[3];	 	 /* array for y-coordinate at last press */
static unsigned rlscnt[3]; 	 /* array for count of button releases */
static unsigned x_lstrls[3];	 /* array for x-coordinate at last release */
static unsigned y_lstrls[3];	 /* array for y-coordinate at last release */
unsigned h_micks, v_micks;

union REGS r;

void cmouses(unsigned *func, unsigned *arg2, unsigned *arg3, unsigned *arg4)
{
	switch(*func) {
		case 0:
			*func = m_reset();
			break;
		case 1:
			m_showcur();
			break;
		case 2:
			m_hidecur();
			break;
		case 3:
			*arg2 = m_readloc();
            *arg3 = mse_xin;
			*arg4 = mse_yin;
			break;
		case 4:
            m_setloc(*arg3, *arg4);
			break;
		case 5:
			*func = m_keyprs(*arg2);
			*arg3 = x_lstprs[*arg2];
			*arg4 = y_lstprs[*arg2];
			*arg2 = prscnt[*arg2];
			break;
		case 6:
			*func = m_keyrls(*arg2);
			*arg3 = x_lstprs[*arg2];
			*arg4 = y_lstprs[*arg2];
			*arg2 = prscnt[*arg2];
			break;
		case 7:
			m_setxlim(*arg3, *arg4);
			break;
		case 8:
			m_setylim(*arg3, *arg4);
			break;
		case 9:
			m_defgcur(*arg3, *arg4, *arg2);
			break;
		case 10:
			m_deftcur(*arg2, *arg3, *arg4);
            break;
		case 11:
        	m_mickeys();
            *arg4 = h_micks;
			*arg3 = v_micks;
			break;
		case 12:
			m_inpmsk(*arg3, *arg4);
			break;
		case 13:
            m_penon();
			break;
		case 14:
			m_penoff();
			break;
		case 15:
			m_setasp(*arg3, *arg4);
			break;
		default: break;
	}
}


/***************************************************************/

unsigned m_reset(void)     /* reset mouse driver and return status of mouse */
{
   r.x.ax = 0;		    /* set mouse function call 0 */
   int86(51, &r, &r ); 	    /* execute mouse interrupt */
   return(r.x.bx);	    /* return status code */
}

/****************************************************************/

void m_showcur(void)    /* increment internal cursor flag and display cursor */
{
	return;				/* no function  */
}

/****************************************************************/

void m_hidecur(void)    /* decrement internal cursor flag and hide cursor */
{
	return;
}

/****************************************************************/

unsigned m_readloc(void)    /* get cursor position and button status */
{
   r.x.ax = 3;		    /* set mouse function call 3 */
   int86( 51, &r, &r ); 	    /* execute mouse interrupt */
   mse_xin = r.x.cx;	    /* store new x-coordinate */
   mse_yin = r.x.dx;	    /* store new y-coordinate */
   return(r.x.bx);	    /* return button status */
}

/****************************************************************/

void m_setloc(unsigned x, unsigned y)	  /* set mouse cursor location */
{

   r.x.ax = 4;		      /* set mouse function call 4 */
   r.x.cx = x;		      /* assign new x-coordinate */
   r.x.dx = y;		      /* assign new y-coordinate */
   int86( 51, &r, &r ); 	      /* execute mouse interrupt */
}

/******************************************************************/

unsigned m_keyprs(unsigned button)    /* get button press information */
						/* button is parameter for left or right button */
{

   r.x.ax = 5;		/* set mouse function call 5 */
   r.x.bx = button;	/* pass parameter for left or right button */
   int86( 51, &r, &r ); 	/* execute mouse interrupt */
   prscnt[button] = r.x.bx;    /* store updated press count */
   x_lstprs[button] = r.x.cx;  /* x-coordinate at last press */
   y_lstprs[button] = r.x.dx;  /* y-coordinate at last press */
   return(r.x.ax);	     /* return button status */
}

/******************************************************************/

unsigned m_keyrls(unsigned button)    /* get button release information */
{

   r.x.ax = 6;		/* set mouse function call 6 */
   r.x.bx = button;	/* pass parameter for left or right button */
   int86( 51, &r, &r ); 	/* execute mouse interrupt */
   rlscnt[button] = r.x.bx;    /* store updated release count */
   x_lstrls[button] = r.x.cx;  /* x-coordinate at last release */
   y_lstrls[button] = r.x.dx;  /* y-coordinate at last release */
   return(r.x.ax);	     /* return button status */
}

/******************************************************************/

void m_setxlim(unsigned xmin,unsigned xmax)	/* set horizontal minimum and maximum coordinates */
/* xmin = minimum x-coordinate for mouse activity */
/* xmax = maximum x-coordinate for mouse activity */
{

   r.x.ax = 7;		/* set mouse function call 7 */
   r.x.cx = xmin; 	/* load horizontal minimum parameter */
   r.x.dx = xmax; 	/* load horizontal maximum parameter */
   int86( 51, &r, &r ); 	/* execute mouse interrupt */
}

/******************************************************************/

void m_setylim(unsigned ymin,unsigned ymax)	/* set vertical minimum and maximum coordinates */
/* ymin = minimum y-coordinate for mouse activity */
/* ymax = maximum y-coordinate for mouse activity */
{

   r.x.ax = 8;		/* set mouse function call 8 */
   r.x.cx = ymin; 	/* load vertical minimum parameter */
   r.x.dx = ymax; 	/* load vertical maximum parameter */
   int86( 51, &r, &r ); 	/* execute mouse interrupt */
}

/******************************************************************/

void m_defgcur(unsigned hot_x,unsigned hot_y,unsigned cur_shape)   /* define graphics cursor attributes */
/* hot_x     = cursor field x-coordinate hot-spot */
/* hot_y     = cursor field y-coordinate hot-spot */
/* cur_shape = pointer to array of cursor and screen masks */
{

   r.x.ax = 9;		/* set mouse function call 9 */
   r.x.bx = hot_x;	/* load horizontal hot-spot */
   r.x.cx = hot_y;	/* load vertical hot-spot */
   r.x.dx = cur_shape;	/* load cursor array pointer address */
   int86( 51, &r, &r ); 	/* execute mouse interrupt */
}

/******************************************************************/

void m_deftcur(unsigned cur_type,unsigned scr_msk,unsigned cur_msk)	/* define text cursor type and masks */
/* cur_type = parameter for software or hardware cursor */
/* scr_msk  = screen mask value - scan line start */
/* cur_msk  = cursor mask value - scan line stop  */
{

   r.x.ax = 10;		/* set mouse function call 10 */
   r.x.bx = cur_type;	/* load cursor type parameter */
   r.x.cx = scr_msk;	/* load screen mask value */
   r.x.dx = cur_msk;	/* load cursor mask value */
   int86( 51, &r, &r ); 	/* execute mouse interrupt */
}

/******************************************************************/

void m_mickeys(void)			 /* read mouse motion counters */
{
   r.x.ax = 11;		/* set mouse function call 11 */
   int86( 51, &r, &r ); 	/* execute mouse interrupt */
   h_micks = r.x.cx;	/* store horizontal motion units */
   v_micks = r.x.dx;	/* store vertical motion units */
}

/******************************************************************/

void m_inpmsk(unsigned call_msk, unsigned offset)	 /* set user defined subroutine and mask */
/* call_msk = call mask - defines interrupt condition */
/* offset   = address offset to user subroutine */
{

   r.x.ax = 12;		/* set mouse function call 12 */
   r.x.cx = call_msk;	/* load call mask value */
   r.x.dx = offset;	/* load user subroutine address */
   int86( 51, &r, &r ); 	/* execute mouse interrupt */
}

/*****************************************************************/

void m_penon(void)		/* turn light-pen emulation on */
{

   r.x.ax = 13;		/* set mouse function call 13 */
   int86( 51, &r, &r ); 	/* execute mouse interrupt */
}

/*****************************************************************/

void m_penoff(void)			 /* turn light-pen emulation off */
{

   r.x.ax = 14;		/* set mouse function call 14 */
   int86( 51, &r, &r ); 	/* execute mouse interrupt */
}

/*****************************************************************/

void m_setasp(unsigned h_ratio,unsigned v_ratio)	 /* set cursor motion counter sensitivity */
/* h_ratio = horizontal mickey:pixel ratio */
/* v_ratio = vertical mickey:pixel ratio */
{

   r.x.ax = 15;		/* set mouse function call 15 */
   r.x.cx = h_ratio;	/* load horizontal ratio value */
   r.x.dx = v_ratio;	/* load vertical ratio value */
   int86( 51, &r, &r ); 	/* execute mouse interrupt */
}

