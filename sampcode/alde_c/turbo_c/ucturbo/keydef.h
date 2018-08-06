/************************************************************************
 *                                                                      *
 *                        keydef.h                                      *
 *                                                                      *
 *     This header defines internal key codes for special extended      *
 *   keys on IBM and Clone computers.  The internal code is found by    *
 *   adding 256 to the actual key code for all special keys.            *
 *                                                                      *
 *                   Copyright (c)  1987 by                             *
 *                      David A.  Miller                                *
 *                      Unicorn Software                                *
 *                                                                      *
 ************************************************************************/


/*  Keyboard Function calls */

#define KBD_READ     0
#define KBD_READY    1
#define KBD_STATUS   2

/*  Define bit masks for particular keys and states */

#define KS_RSHIFT     0x0001
#define KS_LSHIFT     0x0002
#define KS_CONTROL    0x0004
#define KS_ALT        0x0008
#define KS_SL_STATE   0x0010      /* Ctrl-Break on Tandy 1000 */
#define KS_NL_STATE   0x0020
#define KS_CL_STATE   0x0040
#define KS_INS_STATE  0x0080



/* Define function keys first */

#define K_F1   -59
#define K_F2   -60
#define K_F3   -61
#define K_F4   -62
#define K_F5   -63
#define K_F6   -64
#define K_F7   -65
#define K_F8   -66
#define K_F9   -67
#define K_F10  -68

/* Now define the shifted function keys */


#define K_SF1   -84
#define K_SF2   -85
#define K_SF3   -86
#define K_SF4   -87
#define K_SF5   -88
#define K_SF6   -89
#define K_SF7   -90
#define K_SF8   -91
#define K_SF9   -92
#define K_SF10  -93

/* Now for the Control function keys */

#define K_CF1   -94
#define K_CF2   -95
#define K_CF3   -96
#define K_CF4   -97
#define K_CF5   -98
#define K_CF6   -99
#define K_CF7  -100
#define K_CF8  -101
#define K_CF9  -102
#define K_CF10 -103

/* Now for the Alternate function keys */


#define K_AF1  -104
#define K_AF2  -105
#define K_AF3  -106
#define K_AF4  -107
#define K_AF5  -108
#define K_AF6  -109
#define K_AF7  -110
#define K_AF8  -111
#define K_AF9  -112
#define K_AF10 -113

/* Now define the unshifted keypad keys */


#define K_HOME     -71
#define K_END      -79
#define K_PGUP     -73
#define K_PGDN     -81
#define K_LEFT     -75
#define K_RIGHT    -77
#define K_UP       -72
#define K_DOWN     -80




/*  Alternate Cursor Key pad */

#define K_AHOME    -422
#define K_AEND        1
#define K_APGUP       9
#define K_APGDN       3
#define K_ALEFT    -146
#define K_ARIGHT   -234
#define K_AUP      -145
#define K_ADN      -151



/*  Shift Cursor Key pad */

#define K_SHOME     -74
#define K_SEND       49
#define K_SPGUP      57
#define K_SPGDN      51
#define K_SLEFT    -135
#define K_SRIGHT   -136
#define K_SUP      -133
#define K_SDN      -134



/*  Control Cursor Key pad */

#define K_CHOME    -119
#define K_CEND     -117
#define K_CPGUP    -132
#define K_CPGDN    -118
#define K_CLEFT    -115
#define K_CRIGHT   -116

/* Standard Control Key Definitions */

#define K_CTRLA     1
#define K_CTRLB     2
#define K_CTRLC     3
#define K_CTRLD     4
#define K_CTRLE     5
#define K_CTRLF     6
#define K_CTRLG     7
#define K_CTRLH     8
#define K_CTRLI     9
#define K_CTRLJ     10
#define K_CTRLK     11
#define K_CTRLL     12
#define K_CTRLM     13
#define K_CTRLN     14
#define K_CTRLO     15
#define K_CTRLP     16
#define K_CTRLQ     17
#define K_CTRLR     18
#define K_CTRLS     19
#define K_CTRLT     20
#define K_CTRLU     21
#define K_CTRLV     22
#define K_CTRLW     23
#define K_CTRLX     24
#define K_CTRLY     25
#define K_CTRLZ     26

/*  The Alphabetic Alternate Keys  */

#define K_ALTA     30
#define K_ALTB     48
#define K_ALTC     46
#define K_ALTD     32
#define K_ALTE     18
#define K_ALTF     33
#define K_ALTG     34
#define K_ALTH     35
#define K_ALTI     23
#define K_ALTJ     36
#define K_ALTK     37
#define K_ALTL     38
#define K_ALTM     50
#define K_ALTN     49
#define K_ALTO     24
#define K_ALTP     25
#define K_ALTQ     16
#define K_ALTR     19
#define K_ALTS     31
#define K_ALTT     20
#define K_ALTU     22
#define K_ALTV     47
#define K_ALTW     17
#define K_ALTX     45
#define K_ALTY     21
#define K_ALTZ     44

/* Special Alternate Keys */

#define K_ALT1     -120
#define K_ALT2     -121
#define K_ALT3     -122
#define K_ALT4     -123
#define K_ALT5     -124
#define K_ALT6     -125
#define K_ALT7     -126
#define K_ALT8     -127
#define K_ALT9     -128
#define K_ALT0     -129
#define K_ALTDASH  -130
#define K_ALTEQU   -131

/*  Miscellaneous Keys  */

#define K_ESC        27
#define K_SPACE      32
#define K_INS       -82
#define K_DEL       -83
#define K_TAB         9
#define K_BACKTAB    15
#define K_RETURN     13
#define K_SRETURN    13
#define K_CRETURN    10
#define K_CINS     -159
#define K_CDEL     -157
#define K_CBKSPACE  127
#define K_AINS     -160
#define K_ADEL     -158
#define K_ABKSPACE -140
#define K_AESC     -139
#define K_SINS       43
#define K_SDEL       45
#define K_SBKSPACE    8
#define K_CPRINT   -114
#define K_APRINT    -70
#define K_PRINT      16
#define K_ARETURN  -143


