/*
 * EIKON CONTROL LIBRARY - GENERAL HEADER FILE
 *
 * LANGUAGE      : Microsoft C 5.1
 * MODEL         : small or medium
 * ENVIRONMENT   : Microsoft Windows 3.0 SDK
 * STATUS        : operational
 *
 * This header file defines the various identifiers and structures
 * necessary to use one or more controls from the Eikon library of
 * user-defined controls.  
 *
 * Note that this header file assumes that it is listed after
 * the windows include file - windows.h.
 *
 *    Copyright (C) 1989-90
 *    Eikon Systems, Inc.
 *    989 East Hillsdale Blvd, Suite 260
 *    Foster City, California 94404
 * 
 */

/* general size definitions */
#define  CTLTYPES       12          /* number of control types */
#define  CTLDESCR       22          /* size of control menu name */
#define  CTLCLASS       20          /* max size of class name */
#define  CTLTITLE       94          /* max size of control text */

/* ordinal values for functions */
#define  ORDWEP			1           /* window exit function */
#define  ORDINFO        2           /* information function */
#define  ORDSTYLE       3           /* style function */
#define  ORDFLAGS       4           /* control flags function */
#define  ORDWNDFN       5           /* window function */
#define  ORDDLGFN       6           /* style dialog function */

/**/

/*
 * CONTROL STYLE DATA STRUCTURE
 *
 * This data structure is used by the class style dialog function
 * to set and/or reset various control attributes.
 *
 */

typedef struct {
   WORD        wX;                     /* control x origin */
   WORD        wY;                     /* control y origin */
   WORD        wCx;                    /* control width */
   WORD        wCy;                    /* control height */
   WORD        wId;                    /* control id */
   DWORD       dwStyle;                /* control style */
   char        szClass[CTLCLASS];      /* control class name */
   char        szTitle[CTLTITLE];      /* control text */
} CTLSTYLE;

typedef CTLSTYLE *         PCTLSTYLE;
typedef CTLSTYLE FAR *     LPCTLSTYLE;

/**/

/*
 * CONTROL DATA STRUCTURE
 *
 * This data structure is returned by the control options function
 * when enquiring about the capabilities of a particular control.
 * Each control may contain various types (with predefined style
 * bits) under one general class.
 *
 * The width and height fields are used to provide the host
 * application with a suggested size.  The values in these fields
 * reference particular system metric calls.
 *
 */

/* control type definitions */
#define  CT_STD         0x0000  
 
typedef struct {
   WORD        wType;                  /* control type */
   WORD        wWidth;                 /* suggested width (RC units) */
   WORD        wHeight;                /* suggested height (RC units) */
   DWORD       dwStyle;                /* default style */
   char        szDescr[CTLDESCR];      /* menu name */
} CTLTYPE;

typedef struct {
   WORD        wVersion;               /* control version */
   WORD        wCtlTypes;              /* control types */
   char        szClass[CTLCLASS];      /* control class name */
   char        szTitle[CTLTITLE];      /* control title */
   char        szReserved[10];         /* reserved for future use */
   CTLTYPE     Type[CTLTYPES];         /* control type list */
} CTLINFO;

typedef CTLINFO *          PCTLINFO;
typedef CTLINFO FAR *      LPCTLINFO;

/* id to string translation function prototypes */
typedef DWORD  (FAR PASCAL *LPFNSTRTOID)( LPSTR );
typedef WORD   (FAR PASCAL *LPFNIDTOSTR)( WORD, LPSTR, WORD );
