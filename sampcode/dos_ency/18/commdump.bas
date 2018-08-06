' **********************************************************************
' *                                                                    *
' *  COMMDUMP                                                          *
' *                                                                    *
' *  This routine dumps the contents of the COMMSCOP trace buffer to   *
' *  the screen in a formatted manner.  Received data is shown in      *
' *  reverse video.  Where possible, the ASCII character for the byte  *
' *  is shown; otherwise a dot is shown.  The value of the byte is     *
' *  displayed in hex below the character.  Points where tracing was   *
' *  stopped are shown by a solid bar.                                 *
' *                                                                    *
' **********************************************************************

        '
        '  Establish system constants and variables
        '
        DEFINT A-Z

        DIM INREG(7), OUTREG(7)         'Define register arrays

        RAX = 0                         'Establish values for 8086
        RBX = 1                         ' registers
        RCX = 2                         '  .
        RDX = 3                         '  .
        RBP = 4                         '  .
        RSI = 5                         '  .
        RDI = 6                         '  .
        RFL = 7                         '  .

        '
        '  Interrogate COMMSCOP to obtain addresses and count of data in
        '  trace buffer
        '
        INREG(RAX) = &H0300             'Request address data and count
        CALL INT86(&H60, VARPTR(INREG(0)), VARPTR(OUTREG(0)))

        NUM = OUTREG(RCX)               'Number of bytes in buffer
        BUFSEG = OUTREG(RDX)            'Buffer segment address
        BUFOFF = OUTREG(RBX)            'Offset of buffer start

        IF NUM = 0 THEN END

        '
        '  Set screen up and display control data
        '
        CLS
        KEY OFF
        LOCATE 25, 1
        PRINT "NUM ="; NUM;"BUFSEG = "; HEX$(BUFSEG); " BUFOFF = ";
        PRINT HEX$(BUFOFF);
        LOCATE 4, 1
        PRINT STRING$(80,"-")
        DEF SEG = BUFSEG

        '
        '  Set up display control variables
        '
        DLINE = 1
        DCOL = 1
        DSHOWN = 0

        '
        '  Fetch and display each character in buffer
        '
        FOR I= BUFOFF TO BUFOFF+NUM-2 STEP 2
            STAT = PEEK(I)
            DAT = PEEK(I + 1)

            IF (STAT AND 1) = 0 THEN
                COLOR 7, 0
            ELSE
                COLOR 0, 7
            END IF

            RLINE = (DLINE-1) * 4 + 1
            IF (STAT AND &H80) = 0 THEN
                LOCATE RLINE, DCOL
                C$ = CHR$(DAT)
                IF DAT < 32 THEN C$ = "."
                PRINT C$;
                H$ = RIGHT$("00" + HEX$(DAT), 2)
                LOCATE RLINE + 1, DCOL
                PRINT LEFT$(H$, 1);
                LOCATE RLINE + 2, DCOL
                PRINT RIGHT$(H$, 1);
            ELSE
                LOCATE RLINE, DCOL
                PRINT CHR$(178);
                LOCATE RLINE + 1, DCOL
                PRINT CHR$(178);
                LOCATE RLINE + 2, DCOL
                PRINT CHR$(178);
            END IF

            DCOL = DCOL + 1
            IF DCOL > 80 THEN
                COLOR 7, 0
                DCOL = 1
                DLINE = DLINE + 1
                SHOWN = SHOWN + 1
                IF SHOWN = 6 THEN
                    LOCATE 25, 50
                    COLOR 0, 7
                    PRINT "ENTER ANY KEY TO CONTINUE:  ";
                    WHILE LEN(INKEY$) = 0
                    WEND
                    COLOR 7, 0
                    LOCATE 25, 50
                    PRINT SPACE$(29);
                    SHOWN = 0
                END IF
                IF DLINE > 6 THEN
                    LOCATE 24, 1
                    PRINT : PRINT : PRINT : PRINT
                    LOCATE 24, 1
                    PRINT STRING$(80, "-");
                    DLINE = 6
                ELSE
                    LOCATE DLINE * 4, 1
                    PRINT STRING$(80, "-");
                END IF
            END IF

        NEXT I

        END
