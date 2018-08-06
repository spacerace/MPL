              Hercules Monochrome Graphics Card Programming
              ---------------------------------------------

                             provided by

                Robert Morse  MLO5-2/B6  LEWS2::MORSE

The board configuration is controlled by the write-only control register
at port 03BF.  All bits are set to zero upon power-up, which limits
the board to text mode (it will pass IBM monochrome display adapter
diagnostics only while all bits are zero).

Bit     Description
---     -----------
 0      0=disable setting of graphics mode; 1=allow graphics mode.
 1      0=disable page one (allows coexistence with a color graphics
               board); 1=enable page one (required to run Lotus 1-2-3).
 2..7     (not used)



Modes are controlled by the write-only control register at port 03B8. All
bits are set to zero upon power-up. 

Bit     Description
---     -----------
 0        (not used)
 1      0=text; 1=graphics.
 2        (not used)
 3      0=blank screen; 1=activate screen.
 4        (not used)
 5      0=disable text blink; 1=enable text blink.
 6        (not used)
 7      0=page zero at B0000h; 1=page one at B8000h.



Table of 6845 values (all values in hexadecimal): 

Register                        Text    Graphics
-------------------------       ----    --------
 0  Total chars/row -1           61        35	
 1  Visible chars/row            50        2D
 2  Hsync position               52        2E
 3  Hsync width                  0F        07
 4  Total rows -1                19        5B
 5  Additional scan lines        06        02
 6  Visible rows                 19        57
 7  Vsync position               19        57
 8  Interlace control            02        02
 9  Lines/row -1                 0D        03
 A  Top cursor scan              0B        00
 B  Bottom cursor scan           0C        00
 C  Display origin               00        00
 D  Display origin               00        00
 
In text mode, each character time is 0.5625 microseconds and a character is
9 dots wide and 14 dots high.  The controller is programmed for 25
displayed rows of 14 scan lines each.  There are 350 visible scan lines and
370 total scan lines. 


In graphics mode, each character time is 1.000 microseconds and a character
is 16 dots wide and 4 dots high.  The controller is programmed for 87
displayed rows of 4 scan lines each.  There are 348 visible scan lines and
370 total scan lines.  Each row has 45 displayed characters of 16 bits,
giving 720 dots/row.


<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
                      Note on Addressing the Screen
                      -----------------------------

The preceding paragraph gives a hint about pixel addresses in graphics 
mode.  I looked through the HERCBIOS sources and then wrote a program
(HERCDEMO.ASM) to help me understand this.  Here's how screen addressing
works:
      
      o Although you might think of graphics mode as pure bits, the 6845
        chip always thinks in term of characters.  When you switch to
        graphics mode, the characters are 16 bits wide by 4 bits high.
        Thus, in graphics mode the screen has 87 rows of "characters"
        and each row is 45 characters wide, for 720x348 pixels.
      
      o For a reason unknown to me (probably speed of memory access),
        each of the four scan lines in a graphics character is stored in
        a different "bank" or 2000h section of the screen buffer.  All
        87 scan line 0s are stored from 0-2000h, all scan line 1s from
        2000h-4000h, all scan line 2s from 4000h-6000h, and all scan
        line 3s from 6000h-8000h.
      
      o Within a bank of memory (representing one set of scan lines),
        each row is 45 words or 90 bytes wide.  Rows follow each other
        with no intervening space. So, assuming row numbers from 0 to
        86, the 5th graphics character in row 40 is composed of four
        scan lines, each 16 bits wide, located at the following memory
        addresses (offsets from the beginning of the screen buffer):
                40*90 = 3600    (offset in any bank to row 40)
                4*2=8           (5th graphics char = (n-1)*2 bytes/char)
                Scan line 0:    locations 3608,3609
                Scan line 1:    2000h (8192) + 3608,9 = 11800,11801
                Scan line 3:    4000h (16384) + 3608,9 = 19992,19993
                Scan line 4:    6000h (24576) + 3608,9 = 28184,28185
      
      o Computing a pixel location is a little harder, but still fairly
        straightforward.  Say you want to address the bit at 300,300
        with a point of origin at the upper left.  300 scan lines down
        from the top is 300/4 = 75 (no remainder), which is scan line 0
        of row 75.  300 bits into the line is 300/8 = 37 remainder 4.
        The bit you want is in bank 0 (0-2000h) since it is scan line 0,
        offset by 75*90 =6750 for the row, plus 37 for the byte within
        the row, or byte offset 6787 from the beginning of that page in
        screen memory.  Within that byte, you want to set/reset bit 4.
      
      
Well, that should cover it.  If you should discover that I got any of
this wrong, send me mail.
      
-Reid Brown
DECWET::BROWN
