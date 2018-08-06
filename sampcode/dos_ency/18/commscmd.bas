' ************************************************************************
' *                                                                      *
' *  COMMSCMD                                                            *
' *                                                                      *
' *  This routine controls the COMMSCOP program that has been in-        *
' *  stalled as a resident routine.  The operation performed is de-      *
' *  termined by the command line.  The COMMSCMD program is invoked      *
' *  as follows:                                                         *
' *                                                                      *
' *             COMMSCMD [[cmd][,port]]                                  *
' *                                                                      *
' *  where cmd is the command to be executed                             *
' *             STOP   -- stop trace                                     *
' *             START  -- flush trace buffer and start trace             *
' *             RESUME -- resume a stopped trace                         *
' *        port is the COMM  port to be traced (1=COM1, 2=COM2, etc.)    *
' *                                                                      *
' *  If cmd is omitted, STOP is assumed.  If port is omitted, 1 is       *
' *  assumed.                                                            *
' *                                                                      *
' ************************************************************************

        '
        '  Establish system constants and variables
        '
        DEFINT A-Z

        DIM INREG(7), OUTREG(7)         'Define register arrays

        RAX = 0                         'Establish values for 8086
        RBX = 1                         '  registers
        RCX = 2                         '  .
        RDX = 3                         '  .
        RBP = 4                         '  .
        RSI = 5                         '  .
        RDI = 6                         '  .
        RFL = 7                         '  .

        DIM TEXT$(2)

        TEXT$(0) = "STOPPED"
        TEXT$(1) = "STARTED"
        TEXT$(2) = "RESUMED"

        '
        '  Process command-line tail
        '
        C$ = COMMAND$                   'Get command-line data

        IF LEN(C$) = 0 THEN             'If no command line specified
            CMD = 0                     'Set CMD to STOP
            PORT = 0                    'Set PORT to COM1
            GOTO SENDCMD
        END IF

        COMMA = INSTR(C$, ", ")         'Extract operands
        IF COMMA = 0 THEN
            CMDTXT$ = C$
            PORT = 0
        ELSE
            CMDTXT$ = LEFT$(C$, COMMA - 1)
            PORT = VAL(MID$(C$, COMMA + 1)) - 1
        END IF

        IF PORT < 0 THEN PORT = 0

        IF CMDTXT$ = "STOP" THEN
            CMD = 0
        ELSEIF CMDTXT$ = "START" THEN
            CMD = 1
        ELSEIF CMDTXT$ = "RESUME" THEN
            CMD = 2
        ELSE
            CMD = 0
        END IF

        '
        '  Send command to COMMSCOP routine
        '
SENDCMD:
        INREG(RAX) = 256 * CMD
        INREG(RDX) = PORT
        CALL INT86(&H60, VARPTR(INREG(0)), VARPTR(OUTREG(0)))
        '
        '  Notify user that action is complete
        '
        PRINT : PRINT
        PRINT "Communications tracing "; TEXT$(CMD);
        IF CMD <> 0 THEN
            PRINT " for port COM"; MID$(STR$(PORT + 1), 2); ":"
        ELSE
            PRINT
        END IF

        END
