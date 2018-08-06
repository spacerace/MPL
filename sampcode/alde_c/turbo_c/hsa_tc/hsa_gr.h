/************************************************************************/
/*																		*/
/*		H S A _ G R . H  -  Define HSA_GRAF display routines			*/
/*																		*/
/************************************************************************/

/************************************************************************/
/*																		*/
/*		Copyright (c) Hardwood Software Associates, 1986				*/
/*																		*/
/************************************************************************/

/*************************************************************************
 *				H S A _ G R . H   E D I T   L O G 
 *
 * $Log:   D:/CRT/HSA_GR.H_V  $
 *	
 *	   Rev 1.0   11 Aug 1987 11:47:42   R. Evans
 *	Initial revision.
 *	
 ************************************************************************/

#ifndef GR_
#define GR_

#ifndef HSA_ATTRIBUTES
#define HSA_ATTRIBUTES
#define SCR_BASE 0xB000							/* Screen memory base address */
												/* Monochrome Attributes */
#define SCR_NORM 0x7							/* Normal characters */
#define SCR_BOLD 0xF							/* Bold characters */
#define SCR_BLINK 0x87							/* Blinking characters */
#define SCR_REVERSE 0x70						/* Reverse video characters */
#define SCR_UNDER 1								/* Underlined characters */
#define SCR_B_B 0x8F							/* Blinking bold characters */
#define SCR_B_U 0x9								/* Blinking underlined characters */
												/* Colors */
#define SCR_BLACK	0x0
#define SCR_BLUE	0x1
#define SCR_GREEN	0x2
#define SCR_CYAN	0x3
#define SCR_RED		0x4
#define SCR_MAGENTA	0x5
#define SCR_YELLOW	0x6
#define SCR_WHITE	0x7

#define SCR_INTENSE_BIT	0x08					/* Intensty bit */
#define SCR_BLINK_BIT	0x80					/* Bold bit */
												/* Create attribute byte */
#define MAKE_ATTR(fore,back)	(((back)<<4) | (fore))
												/* CRT states */
#define MONOCHROME	0x7
#define CGA			0x6
#define EGA			0x10
#endif

#ifdef LINT_ARGS
int g_state (void);
void g_herc (void);
void g_cga_hr (void);
void g_ega_hr_co (void);
void g_other (int,int,int,unsigned int,int);
void g_delay (int);
int disp_graph (void);
void disp_text();
void g_loc (int *, int *);
int g_r_origin (void);
int g_r_point (int,int);
void g_str_h (int,int, char *);
void g_fill (int,int,int);
void g_save (int *,int);
void g_restore (int *,int);
int g_origin (int);
void g_bs_h (void);
void g_clr (void);
void g_solid (void);
void g_dot (void);
void g_dash (void);
void g_set_line (unsigned int);
void g_fore_color (int);
void g_back_color (int);
void g_reverse_color (void);
void g_mode (int);
void g_point (int,int);
void g_line (int,int,int,int);
void g_scale (int,int);
void gs_loc (int *,int *);
void gs_point (int,int);
int gs_r_point (int,int);
void gs_line (int,int,int,int);
void gs_str_h (int,int, char *);
int gs_arc (int,int,int,int,int,int,int);
int g_page (int);
char *g_gets (char *);
#else
int g_state ();
void g_herc ();
void g_cga_hr ();
void g_ega_hr_co ();
void g_other ();
void g_delay ();
int disp_graph ();
void disp_text();
void g_loc ();
int g_r_origin ();
int g_r_point ();
void g_str_h ();
void g_fill ();
void g_save ();
void g_restore ();
int g_origin ();
void g_bs_h ();
void g_clr ();
void g_solid ();
void g_dot ();
void g_dash ();
void g_set_line ();
void g_fore_color ();
void g_back_color ();
void g_reverse_color ();
void g_mode ();
void g_point ();
void g_line ();
void g_scale ();
void gs_loc ();
void gs_point ();
int gs_r_point ();
void gs_line ();
void gs_str_h ();
int gs_arc ();
int g_page ();
char *g_gets ();

#endif
#endif



