'Binary-to-Hex file conversion utility.
'Requires Microsoft QuickBASIC version 3.0 or later.

DEFINT A-Z                                    ' All variables are integers
                                              ' unless otherwise declared.
CONST FALSE = 0                               ' Value of logical FALSE.
CONST TRUE = NOT FALSE                        ' Value of logical TRUE.

DEF FNHXB$(X) = RIGHT$(HEX$(&H100 + X), 2)    ' Return 2-digit hex value for X.
DEF FNHXW$(X!) = RIGHT$("000" + HEX$(X!), 4)  ' Return 4-digit hex value for X!.
DEF FNMOD(X, Y) = X! - INT(X!/Y) * Y          ' X! MOD Y (the MOD operation is
                                              ' only for integers).
CONST SRCCNL = 1                              ' Source (.BIN) file channel.
CONST TGTCNL = 2                              ' Target (.HEX) file channel.

LINE INPUT "Enter full name of source .BIN file        :  ";SRCFIL$
OPEN SCRCFIL$ FOR INPUT AS SRCCNL             ' Test for source (.BIN) file.
SRCSIZ! = LOF(SRCCNL)                         ' Save file's size.
CLOSE SRCCNL
IF (SRCSIZ! > 65536) THEN                     ' Reject if file exceeds 64 KB.
    PRINT "Cannot convert file larger than 64 KB."
    END
END IF

LINE INPUT "Enter full name of target .HEX file        :  ";TGTFIL$
OPEN TGTFIL$ FOR OUTPUT AS TGTCNL             ' Test target (.HEX) filename.
CLOSE TGTCNL

DO
    LINE INPUT "Enter starting address of .BIN file in HEX :  ";L$
    ADRBGN! = VAL("&H" + L$)                  ' Convert ASCII HEX address value
                                              ' to binary value.
    IF (ADRBGN! < 0) THEN                     ' HEX values 8000-FFFFH convert
     ADRBGN! = 65536 + ADRBGN!                ' to negative values.
    END IF
    ADREND! = ADRBGN! + SRCSIZ! - 1           ' Calculate resulting end address.
    IF (ADREND! > 65535) THEN                 ' Reject if address exceeds FFFFH.
     PRINT "Entered start address causes end address to exceed FFFFH."
    END IF
LOOP UNTIL (ADRFLD! >= 0) AND (ADRFLD! <= 65535) AND (ADREND! <= 65535)

DO
    LINE INPUT "Enter byte count for each record in HEX    :  ";L$
    SRCRLN = VAL("&H" + L$)                   ' Convert ASCII HEX max record
                                              ' length value to binary value.
    IF (SRCRLN < 0) THEN                      ' HEX values 8000H-FFFFH convert
     SRCRLN = 65536 + SRCRLN                  ' to negative values.
    END IF
LOOP UNTIL (SRCRLN > 0) AND (SRCRLN < 256)    ' Ask again if not 1-255.

OPEN SRCFIL$ AS SRCCNL LEN = SRCRLN           ' Reopen source for block I/O.
FIELD#SRCCNL,SRCRLN AS SRCBLK$
OPEN TGTFIL$ FOR OUTPUT AS TGTCNL             ' Reopen target for text output.
SRCREC = 0                                    ' Starting source block # minus 1.

FOR ADRFLD! = ADRBGN! TO ADREND! STEP SRCRLN  ' Convert one block per loop.
    SRCREC = SRCREC + 1                       ' Next source block.
    GET SRCCNL,SRCREC                         ' Read the source block.
    IF (ADRFLD! + SRCRLN > ADREND!) THEN      ' If last block less than full
     BLK$=LEFT$(SRCBLK$,ADREND!-ADRFLD!+1)    ' size:  trim it.
    ELSE                                      ' Else:
        BLK$ = SRCBLK$                        ' Use full block.
    END IF

    PRINT#TGTCNL, ":";                        ' Write record mark.

    PRINT#TGTCNL, FNHXB$(LEN(BLK$));          ' Write data field size.
    CHKSUM = LEN(BLK$)                        ' Initialize checksum accumulate
                                              ' with first value.
    PRINT#TGTCNL,FNHXW$(ADRFLD!);             ' Write record's load address.

' The following "AND &HFF" operations limit CHKSUM to a byte value.
    CHKSUM = CHKSUM + INT(ADRFLD!/256) AND &HFF   ' Add hi byte of adrs to csum.
    CHKSUM = CHKSUM + FNMOD(ADRFLD!,256) AND &HFF ' Add lo byte of adrs to csum.

    PRINT#TGTCNL,FNHXB$(0);                   ' Write record type.

' Don't bother to add record type byte to checksum since it's 0.
    FOR IDX = 1 TO LEN(BLK$)                  ' Write all bytes.
     PRINT#TGTCNL,FNHXB$(ASC(MID$(BLK$,IDX,1)));      ' Write next byte.
     CHKSUM = CHKSUM + ASC(MID$(BLK$,IDX,1)) AND &HFF ' Incl byte in csum.
    NEXT IDX

    CHKSUM = 0 - CHKSUM AND &HFF              ' Negate checksum then limit
                                              ' to byte value.
    PRINT #TGTCNL,FNHXB$(CHKSUM)              ' End record with checksum.

NEXT ADRFLD!

PRINT#TGTCNL, ":00000001FF"                   ' Write end-of-file record.

CLOSE TGTCNL                                  ' Close target file.
CLOSE SRCCNL                                  ' Close source file.

END
