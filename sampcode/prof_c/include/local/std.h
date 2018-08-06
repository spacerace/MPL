/*
 *	std.h
 */

/* data type aliases */
#define	META	short
#define UCHAR	unsigned char
#define UINT	unsigned int
#define ULONG	unsigned long
#define USHORT	unsigned short

/* Boolean data type */
typedef enum {
	FALSE, TRUE
} BOOLEAN;

/* function return values and program exit codes */
#define OK	0
#define BAD	1
#define SUCCESS	0
#define FAILURE	1

/* infinite loop */
#define FOREVER	while (1)

/* masks */
#define HIBYTE	0xFF00
#define LOBYTE	0x00FF
#define ASCII	0x7F
#define HIBIT	0x80

/* lengths */
#define MAXNAME	8
#define MAXEXT	3
#define	MAXLINE	256
#define	MAXPATH	64

/* special number */
#define BIGGEST	65535
