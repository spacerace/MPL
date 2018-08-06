/*************************************************************************
 *                            border.h                                   *
 *                                                                       *
 *    This header defines several border types that may be used in       *
 *    building window programs.   The default attribute is either        *
 *    gray or reverse gray.  This will work with monochrome monitors.    *
 *    The default attribute may be changed be re-defining it within      *
 *    your program.  In addition, other border types may also be         *
 *    defined in your program in a manner similar to these examples.     *
 *                                                                       *
 *************************************************************************/



BORDER bdr1   = { 0x0da, 0x0bf, 0x0c0, 0x0d9, 0x0b3, 0x0c4, 0x07 };
BORDER bdr1r  = { 0x0da, 0x0bf, 0x0c0, 0x0d9, 0x0b3, 0x0c4, 0x70 };
BORDER bdr2   = { 0x0c9, 0x0bb, 0x0c8, 0x0bc, 0x0ba, 0x0cd, 0x0f };
BORDER bdr2r  = { 0x0c9, 0x0bb, 0x0c8, 0x0bc, 0x0ba, 0x0cd, 0xf0 };
BORDER bdr12  = { 0x0d5, 0x0b8, 0x0d4, 0x0be, 0x0b3, 0x0cd, 0x07 };
BORDER bdr12r = { 0x0d5, 0x0b8, 0x0d4, 0x0be, 0x0b3, 0x0cd, 0x70 };
BORDER bdr21  = { 0x0d6, 0x0b7, 0x0d3, 0x0bd, 0x0ba, 0x0c4, 0x07 };
BORDER bdr21r = { 0x0d6, 0x0b7, 0x0d3, 0x0bd, 0x0ba, 0x0c4, 0x70 };
BORDER star   = { '*',   '*',   '*',   '*',   '*',   '*',   0x07 };
BORDER starr  = { '*',   '*',   '*',   '*',   '*',   '*',   0x70 };
BORDER dot    = { 'ú',   'ú',   'ú',   'ú',   'ú',   'ú',   0x07 };
BORDER dotr   = { 'ú',   'ú',   'ú',   'ú',   'ú',   'ú',   0x70 };




