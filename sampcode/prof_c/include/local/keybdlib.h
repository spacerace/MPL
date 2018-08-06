/*
 *	keybdlib.h
 */

/* keyboard routine numbers */
#define KBD_READ	0
#define KBD_READY	1
#define KBD_STATUS	2

/* bit masks for keys and states */
#define KS_RSHIFT	0x0001
#define KS_LSHIFT	0x0002
#define KS_CONTROL	0x0004
#define KS_ALT		0x0008
#define KS_SL_STATE	0x0010
#define KS_NL_STATE	0x0020
#define KS_CL_STATE	0x0040
#define KS_INS_STATE	0x0080
