/****************************************************************************
 *                                bdrdef.h                                  *
 *                                                                          *
 *   Basic bordef defintion header for the C Tool Box.  This header         *
 *   contains definitions of several basic border types.  In no border      *
 *   type is specified the library will default to a double line border.    *
 *                                                                          *
 *                                                                          *
 *                           Copyright (c) 1987                             *
 *                            David A.  Miller                              *
 *                            Unicorn Software                              *
 *                                                                          *
 ****************************************************************************/



BORDER bdr1   = { 'Ä', '³', 'Ú', '¿', 'À', 'Ù' };

BORDER bdr12  = { 'Ä', 'º', 'Ö', '·', 'Ó', '½'  };

BORDER bdr21  = { 'Í', '³', 'Õ', '¸', 'Ô', '¾'  };

BORDER star   = { '*',   '*',   '*',   '*',   '*',   '*'   };

BORDER dot    = { 'ú',   'ú',   'ú',   'ú',   'ú',   'ú'   };

BORDER solid  = { 'Û', 'Û','Û','Û','Û','Û' };

BORDER heart  = { '\003','\003', '\003', '\003', '\003', '\003' };

BORDER space  = { ' ', ' ',' ',' ',' ',' ' };
