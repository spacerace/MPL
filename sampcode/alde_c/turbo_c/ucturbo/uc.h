/***************************************************************************
 *                                                                         *
 *                             uc.h                                        *
 *                                                                         *
 *      The unicorn library header for version 4.0                         *
 *                                                                         *
 ***************************************************************************/

 /*  alias data types  */

#define META    short
#define UCHAR   unsigned char
#define UINT    unsigned int
#define ULONG   unsigned long
#define USHORT  unsigned short

/*  Boolean data type  */

typedef enum
{
   FALSE, TRUE
} BOOLEAN;

/* Return values and exit codes */

#define OK       0
#define BAD      1
#define SUCCESS  0
#define FAILURE  1

/*  infinite loop declaration */

#define FOREVER   1

/* masks */

#define HIBYTE    0xff00
#define LOBYTE    0x00ff
#define HI_NIBBLE 0x0f
#define LO_NIBBLE 0xf0
#define ASCII     0x7f
#define HIBIT     0x80

/* data lengths */

#define MAXNAME  8
#define MAXEXT   3
#define MAXLINE  256
#define MAXPATH  64

/* special values */

#define BIGGEST  65535


/*  macro Unit conversions */

#define INCH_CM(x)        ((x)*2.54)
#define CM_INCH(x)        ((x)*0.3937008)
#define DEG_RAD(x)        ((x)*0.017453293)
#define RAD_DEG(x)        ((x)*57.29577951)
#define OZ_GM(x)          ((x)*28.349523)
#define GM_OZ(x)          ((x)*0.035274)
#define GAL_LT(x)         ((x)*3.78541)
#define LT_GAL(x)         ((x)*0.2642722)
#define LB_KG(x)          ((x)*0.45359237)
#define KG_LB(x)          ((x)*2.20462262)

