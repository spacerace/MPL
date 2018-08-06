/*
 * FORM LIBRARY - HEADER FILE
 *
 * LANGUAGE      : Microsoft C 5.1
 * MODEL         : medium
 * ENVIRONMENT   : Microsoft Windows 3.0 SDK
 * STATUS        : operational
 *
 *    Eikon Systems, Inc.
 *    989 East Hillsdale Blvd, Suite 260
 *    Foster City, California 94404
 *    415-349-4664
 *
 * 07/12/90 1.00 - Kevin P. Welch - initial creation.
 *
 */

/* dialog box definitions */
#define  NAME           100
#define  COMPANY        101
#define  ADDRESS        102
#define  CITY           103
#define  STATE          104
#define  ZIPCODE        105

#define  PRODUCT        106
#define  SHIPVIA        107
#define  PAYMETHOD      108
#define  CARDNUM        109
#define  EXPDATE        110

#define  OPTION1        111
#define  OPTION2        112
#define  FRIEND         113
#define  AD             114
#define  SHOW           115
#define  DEALER         116
#define  REVIEW         117
#define  ARTICLE        118
#define  COMMENT        119

/* functions */
HANDLE FAR PASCAL FormEdit( HWND, HANDLE );
HANDLE FAR PASCAL FormPrint( HWND, HANDLE );
