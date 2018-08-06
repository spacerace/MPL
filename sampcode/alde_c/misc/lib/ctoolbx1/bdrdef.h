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



BORDER bdr1   = { '�', '�', '�', '�', '�', '�' };

BORDER bdr12  = { '�', '�', '�', '�', '�', '�'  };

BORDER bdr21  = { '�', '�', '�', '�', '�', '�'  };

BORDER star   = { '*',   '*',   '*',   '*',   '*',   '*'   };

BORDER dot    = { '�',   '�',   '�',   '�',   '�',   '�'   };

BORDER solid  = { '�', '�','�','�','�','�' };

BORDER heart  = { '\003','\003', '\003', '\003', '\003', '\003' };

BORDER space  = { ' ', ' ',' ',' ',' ',' ' };
