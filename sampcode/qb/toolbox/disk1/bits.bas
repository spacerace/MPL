  ' ************************************************
  ' **  Name:          BITS                       **
  ' **  Type:          Toolbox                    **
  ' **  Module:        BITS.BAS                   **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Demonstrates the bit manipulation functions
  ' and subprograms.
  '
  ' USAGE: No command line parameters
  ' .MAK FILE:       (none)
  ' PARAMETERS:      (none)
  ' VARIABLES:       max%       Upper limit for the prime number generator
  '                  b$         Bit string for finding prime numbers
  '                  n%         Loop index for sieve of Eratosthenes
  '                  bit%       Bit retrieved from b$
  '                  i%         Bit loop index
  '                  q$         The double quote character
  
  
    DECLARE FUNCTION BinStr2Bin% (b$)
    DECLARE FUNCTION Bin2BinStr$ (b%)
  
  ' Subprograms
    DECLARE SUB BitGet (a$, bitIndex%, bit%)
    DECLARE SUB BitPut (b$, bitIndex%, bit%)
  
  ' Prime numbers less than max%, using bit fields in B$
    CLS
    max% = 1000
    PRINT "Primes up to"; max%; "using BitGet and BitPut for sieve..."
    PRINT
    PRINT 1; 2;
    b$ = STRING$(max% \ 8 + 1, 0)
    FOR n% = 3 TO max% STEP 2
        BitGet b$, n%, bit%
        IF bit% = 0 THEN
            PRINT n%;
            FOR i% = 3 * n% TO max% STEP n% + n%
                BitPut b$, i%, 1
            NEXT i%
        END IF
    NEXT n%
    PRINT
  
  ' Demonstration of the Bin2BinStr$ function
    PRINT
    PRINT "Bin2BinStr$(12345) = "; Bin2BinStr$(12345)
  
  ' Demonstration of the BinStr2Bin% function
    PRINT
    q$ = CHR$(34)
    PRINT "BinStr2Bin%("; q$; "1001011"; q$; ") = ";
    PRINT BinStr2Bin%("1001011")
  
  ' That's all
    END
  

  ' ************************************************
  ' **  Name:          Bin2BinStr$                **
  ' **  Type:          Function                   **
  ' **  Module:        BITS.BAS                   **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns a string of sixteen "0" and "1" characters
  ' that represent the binary value of b%.
  '
  ' EXAMPLE OF USE:  PRINT Bin2BinStr$(b%)
  ' PARAMETERS:      b%         Integer number
  ' VARIABLES:       t$         Working string space for forming binary string
  '                  b%         Integer number
  '                  mask%      Bit isolation mask
  '                  i%         Looping index
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Bin2BinStr$ (b%)
  '
    FUNCTION Bin2BinStr$ (b%) STATIC
        t$ = STRING$(16, "0")
        IF b% THEN
            IF b% < 0 THEN
                MID$(t$, 1, 1) = "1"
            END IF
            mask% = &H4000
            FOR i% = 2 TO 16
                IF b% AND mask% THEN
                    MID$(t$, i%, 1) = "1"
                END IF
                mask% = mask% \ 2
            NEXT i%
        END IF
        Bin2BinStr$ = t$
    END FUNCTION

  ' ************************************************
  ' **  Name:          BinStr2Bin%                **
  ' **  Type:          Function                   **
  ' **  Module:        BITS.BAS                   **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the integer represented by a string of up
  ' to 16 "0" and "1" characters.
  '
  ' EXAMPLE OF USE:  PRINT BinStr2Bin%(b$)
  ' PARAMETERS:      b$         Binary representation string
  ' VARIABLES:       bin%       Working variable for finding value
  '                  t$         Working copy of b$
  '                  mask%      Bit mask for forming value
  '                  i%         Looping index
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION BinStr2Bin% (b$)
  '
    FUNCTION BinStr2Bin% (b$) STATIC
        bin% = 0
        t$ = RIGHT$(STRING$(16, "0") + b$, 16)
        IF LEFT$(t$, 1) = "1" THEN
            bin% = &H8000
        END IF
        mask% = &H4000
        FOR i% = 2 TO 16
            IF MID$(t$, i%, 1) = "1" THEN
                bin% = bin% OR mask%
            END IF
            mask% = mask% \ 2
        NEXT i%
        BinStr2Bin% = bin%
    END FUNCTION

  ' ************************************************
  ' **  Name:          BitGet                     **
  ' **  Type:          Subprogram                 **
  ' **  Module:        BITS.BAS                   **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Extracts the bit at bitIndex% into a$ and returns
  ' either 0 or 1 in bit%.  The value of bitIndex%
  ' can range from 1 to 8 * LEN(a$).
  '
  ' EXAMPLE OF USE:  BitGet a$, bitIndex%, bit%
  ' PARAMETERS:      a$         String where bit is stored
  '                  bitIndex%  Bit position in string
  '                  bit%       Extracted bit value, 0 or 1
  ' VARIABLES:       byte%      Byte location in string of the bit
  '                  mask%      Bit isolation mask for given bit
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB BitGet (a$, bitIndex%, bit%)
  '
    SUB BitGet (a$, bitIndex%, bit%) STATIC
        byte% = (bitIndex% - 1) \ 8 + 1
        SELECT CASE bitIndex% MOD 8
        CASE 1
            mask% = 128
        CASE 2
            mask% = 64
        CASE 3
            mask% = 32
        CASE 4
            mask% = 16
        CASE 5
            mask% = 8
        CASE 6
            mask% = 4
        CASE 7
            mask% = 2
        CASE 0
            mask% = 1
        END SELECT
        IF ASC(MID$(a$, byte%, 1)) AND mask% THEN
            bit% = 1
        ELSE
            bit% = 0
        END IF
    END SUB

  ' ************************************************
  ' **  Name:          BitPut                     **
  ' **  Type:          Subprogram                 **
  ' **  Module:        BITS.BAS                   **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' If bit% is non-zero, then the bit at bitIndex% into
  ' a$ is set to 1; otherwise, it's set to 0. The value
  ' of bitIndex% can range from 1 to 8 * LEN(a$).
  '
  ' EXAMPLE OF USE:  BitPut a$, bitIndex%, bit%
  ' PARAMETERS:      a$         String containing the bits
  '                  bitIndex%  Index to the bit of concern
  '                  bit%       Value of bit (1 to set, 0 to clear)
  ' VARIABLES:       bytePtr%   Pointer to the byte position in the string
  '                  mask%      Bit isolation mask
  '                  byteNow%   Current numeric value of string byte
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB BitPut (b$, bitIndex%, bit%)
  '
    SUB BitPut (a$, bitIndex%, bit%) STATIC
        bytePtr% = bitIndex% \ 8 + 1
        SELECT CASE bitIndex% MOD 8
        CASE 1
            mask% = 128
        CASE 2
            mask% = 64
        CASE 3
            mask% = 32
        CASE 4
            mask% = 16
        CASE 5
            mask% = 8
        CASE 6
            mask% = 4
        CASE 7
            mask% = 2
        CASE 0
            mask% = 1
            bytePtr% = bytePtr% - 1
        END SELECT
        byteNow% = ASC(MID$(a$, bytePtr%, 1))
        IF byteNow% AND mask% THEN
            IF bit% = 0 THEN
                MID$(a$, bytePtr%, 1) = CHR$(byteNow% XOR mask%)
            END IF
        ELSE
            IF bit% THEN
                MID$(a$, bytePtr%, 1) = CHR$(byteNow% XOR mask%)
            END IF
        END IF
    END SUB

