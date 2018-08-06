/*
**          SM.LIB  function source code
**  Copyright 1986, S.E. Margison
**
**  FUNCTION: screen.h
** extra defines for screen handling operations
*/

#define BW40 0   /* b&w 40 x 25 screen */
#define CLR40 1  /* color 40 x 25 screen */
#define BW80 2   /* b&w 80 x 25 screen */
#define CLR80 3  /* color 80 x 25 screen */
#define CG320 4  /* color graphics, 320 x 200 */
#define BW320 5  /* b&w graphics, 320 x 200 */
#define BW640 6  /* b&w graphics, 640 x 200 */
#define MONO 7   /* 80 x 25 b&w using Monochrome card */

/* video attributes */
#define BLINKING 0x87
#define REVERSE 0x70
#define REVBLINK 0xf0
#define NORMAL 0x07
#define HIGHLITE 0x0f
#define HIGHBLINK 0x8f
#define BLINKBIT 0x80   /* OR in to cause blink */
#define HILTBIT 0x08    /* OR in to cause highlight */

/* colors -- Use as is for foreground colors
**           For background, shift left by 4 and OR with
**           foreground and possible video attributes
*/
#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define BROWN 6
#define WHITE 7
#define GRAY 8
#define LTBLUE 9
#define LTGREEN 10
#define LTCYAN 11
#define LTRED 12
#define LTMAGENTA 13
#define YELLOW 14
#define HIWHITE 15    /* hi-intensity white */
