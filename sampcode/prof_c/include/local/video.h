/*
 *	video.h
 */

/* current video state/mode information */
extern short Vmode;
extern short Vwidth;
extern short Vpage;

#define MAXVMODE  16

/* video limit tables */
extern short Maxrow[MAXVMODE];
extern short Maxcol[MAXVMODE];
extern short Maxpage[MAXVMODE];

/* active display */
#define MONO	1
#define COLOR	2

/* cursor modes */
#define CURSOR_OFF	0
#define CURSOR_ON	1

/* installed display adapters */
#define MDA	1
#define CGA	2
#define EGA	4

/* --- video modes --- */
/* CGA modes */
#define CGA_M40		0
#define CGA_C40		1
#define CGA_M80		2
#define CGA_C80		3
#define CGA_CMRES	4
#define CGA_MMRES	5
#define CGA_MHRES	6
/* MDA mode */
#define MDA_M80		7
/* PCjr modes */
#define PCJR_CLRES	8	
#define PCJR_CMRES	9
#define PCJR_CHRES	10
/* modes 11 and 12 are not currently used */
/* EGA modes */
#define EGA_CMRES	13
#define EGA_CHRES	14
#define EGA_MHRES	15
#define EGA_EHRES	16

/* miscellaneous video masks */
  /* character mask */
#define CMASK	0x00FF
  /* attribute mask */
#define AMASK	0xFF00

/* attribute modifiers */
#define BRIGHT	8
#define BLINK	128

/* primary video attributes */
#define BLU	1
#define GRN	2
#define RED	4

/* composite video attributes */
#define BLK	0
#define CYAN	(BLU | GRN)		/* 3 */
#define MAGENTA	(BLU | RED)		/* 5 */
#define BRN	(GRN | RED)		/* 6 */
#define WHT	(BLU | GRN | RED)	/* 7 */
#define GRAY	(BLK | BRIGHT)
#define LBLU	(BLU | BRIGHT)
#define LGRN	(GRN | BRIGHT)
#define LCYAN	(CYAN | BRIGHT)
#define LRED	(RED | BRIGHT)
#define LMAG	(MAG | BRIGHT)
#define YEL	(BRN | BRIGHT)
#define BWHT	(WHT | BRIGHT)
#define NORMAL	WHT
#define REVERSE	112

/*
 *	drawing characters -- items having two numbers use
 *	the first number as the horizontal specifier
 */

/* single-line boxes */
#define VBAR1	179
#define VLINE	179	/* alias */
#define HBAR1	196
#define HLINE	196	/* alias */
#define	ULC11	218
#define	URC11	191
#define	LLC11	192
#define	LRC11	217
#define	TL11	195
#define	TR11	180
#define	TT11	194
#define	TB11	193
#define X11	197

/* double-line boxes */
#define VBAR2	186
#define HBAR2	205
#define	ULC22	201
#define	URC22	187
#define LLC22	200
#define LRC22	188
#define	TL22	204
#define	TR22	185
#define	TT22	203
#define	TB22	202
#define X22	206

/* single-line horizontal & double-line vertical boxes */
#define	ULC12	214
#define	URC12	183
#define LLC12	211
#define LRC12	189
#define	TL12	199
#define	TR12	182
#define	TT12	210
#define	TB12	208
#define X12	215

/* double-line horizontal & single-line vertical boxes */
#define	ULC21	213
#define	URC21	184
#define LLC21	212
#define LRC21	190
#define	TL21	198
#define	TR21	181
#define	TT21	209
#define	TB21	207
#define X21	216

/* full and partial blocks */
#define BLOCK	219
#define VBAR	219	/* alias */
#define VBARL	221
#define VBARR	222
#define HBART	223
#define HBARB	220

/* special character-graphic symbols */
#define BLANK		32
#define DIAMOND		4
#define UPARROW		24
#define DOWNARROW	25
#define RIGHTARROW	26
#define LEFTARROW	27
#define SLASH		47
