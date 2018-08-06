/*
**          SM.LIB  function source code
**  Copyright 1986, S.E. Margison
**
**  FUNCTION: keys.h
**  extra defines for keyboard operations
**  9-15-86 A
*/

#define ESC 0x1b
#define BACKSP 0x08
#define HTAB 0x09

#define FKLEAD 0   /* if key = 0, read next byte as function key */
#define FK1 59
#define FK2 60
#define FK3 61
#define FK4 62
#define FK5 63
#define FK6 64
#define FK7 65
#define FK8 66
#define FK9 67
#define FK10 68

#define SFK1 84  /* shifted function key */
#define SFK2 85
#define SFK3 86
#define SFK4 87
#define SFK5 88
#define SFK6 89
#define SFK7 90
#define SFK8 91
#define SFK9 92
#define SFK10 93

#define CFK1 94  /* control function key */
#define CFK2 95
#define CFK3 96
#define CFK4 97
#define CFK5 98
#define CFK6 99
#define CFK7 100
#define CFK8 101
#define CFK9 102
#define CFK10 103

#define AFK1 104  /* alt function keys */
#define AFK2 105
#define AFK3 106
#define AFK4 107
#define AFK5 108
#define AFK6 109
#define AFK7 110
#define AFK8 111
#define AFK9 112
#define AFK10 113

#define DELKEY 83	/* an alternate function key */
#define HOME 71		/* HOME key */
#define CURLEFT 75	/* <- */
#define ENDKEY 79	/* END key */
#define CURUP 72	/* up arrow */
#define CURDN 80	/* down arrow */
#define PGUP 73		/* PgUp */
#define CURRIGHT 77	/* -> */
#define PGDN 81		/* PgDn */
#define INSERT 82	/* Ins */
#define CTRLHOME 119	/* Ctrl Home */
#define CTRLCURLF 115	/* Ctrl <- */
#define CTRLEND 117	/* Ctrl End */
#define CTRLPRTSC 114	/* Ctrl PrtSc */
#define CTRLPGUP 132	/* Ctrl PgUp */
#define CTRLCURRT 116	/* Ctrl -> */
#define CTRLPGDN 118	/* Ctrl PgDn */
#define BKTAB 15	/* Shift Tab */
#define ALTMINUS 130	/* Alt - */
#define ALTEQUAL 131	/* Alt = */

