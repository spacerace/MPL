/*
 *	bioslib.h
 */

/* BIOS interrupts */
#define PRINT_SCRN	0x05
#define TOD_INIT	0x08
#define KEYBD_INIT	0x09
#define DISK_INIT	0x0E
#define VIDEO_IO	0x10
#define EQUIP_CK	0x11
#define MEM_SIZE	0x12
#define DISK_IO		0x13
#define RS323_IO	0x14
#define CASS_IO		0x15
#define KEYBD_IO	0x16
#define PRINT_IO	0x17
#define TOD		0x1A
#define VIDEO_INIT	0x1D
#define GRAPHICS	0x1F

/* video routine numbers */
/* (placed in register AH before a BIOS interrupt 10H */
#define SET_MODE	0
#define CUR_TYPE	1
#define CUR_POS		2
#define GET_CUR		3
#define LPEN_POS	4
#define SET_PAGE	5
#define SCROLL_UP	6
#define SCROLL_DN	7
#define READ_CHAR_ATTR	8
#define WRITE_CHAR_ATTR	9
#define WRITE_CHAR	10
#define PALETTE		11
#define WRITE_DOT	12
#define READ_DOT	13
#define WRITE_TTY	14
#define GET_STATE	15
#define ALT_FUNCTION	18	/* EGA only */
#define WRITE_STR	19	/* AT only */

/* disk routine numbers */
#define RESET_DISK	0
#define DISK_STATUS	1
#define READ_SECTOR	2
#define WRITE_SECTOR	3
#define VERIFY_SECTOR	4
#define FORMAT_TRACK	5

/* keyboard routine numbers */
#define KBD_READ	0
#define KBD_READY	1
#define KBD_STATUS	2
