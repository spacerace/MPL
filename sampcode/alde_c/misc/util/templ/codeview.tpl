; Comment lines begin with ; or *
; All blank lines are ignored
; If you want a blank line in the function or info area, enter // at the
;	beginning of the line
; Maximum number of FUNCTION KEYS = 12
; Maximum lines above the function = 4
; There are three columns of information area
; Maximum characters per column = 35
; Maximum lines of info = 15
; mode lines are .cx (columns 1-3) for the info lines
;  		 .fx (keys 1-15) for the function keys
;		 .t for the title line
;
; The only requirements are: 
;	When a mode is set, the subsequent lines of data are for that mode.  
;		Until the mode is changed.	
;	Text cannot go on the same line as the mode set command
;
; All data does not have to be in any order and may skip around.
;
; Example .c1 sets the mode for data into column 1. The next lines of data
;	will be under that column.  If the same column is set again, the
;       data will continue.

.t
Microsoft CodeView

.c1
CTRL+G Grow (make window larger)
CTRL+T Tiny (make window smaller)

.c2
CTRL+F Find Text
CTRL+W Watch Expession

.c3
CTRL+U Delete Watch

.f1
Get help

.f2
View
registers

.f3
Set source
or assembly
mode

.f4
View
output
screen

.f5
Go

.f6
Change
cursor
window

.f7
Go to
cursor line

.f8
Trace
into

.f9
Set 
breakpoint
at cursor

.f10
Step
over

