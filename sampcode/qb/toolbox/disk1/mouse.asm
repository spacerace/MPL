 ; **********************************************
 ; **  MOUSE.ASM              Macro Assembler  **
 ; **                                          **
 ; **  Assembly subprogram for accessing the   **
 ; **  Microsoft Mouse from QuickBASIC 4.00    **
 ; **                                          **
 ; **  Use:  CALL MOUSE (M1%, M2%, M3%, M4%)   **
 ; **********************************************
 ;
 ; EXAMPLE OF USE:  CALL Mouse (m1%, m2%, m3%, m4%)
 ; PARAMETERS:      m1%        Passed in AX to the mouse driver
 ;                  m2%        Passed in BX to the mouse driver
 ;                  m3%        Passed in CX to the mouse driver
 ;                  m4%        Passed in DX to the mouse driver
 ; VARIABLES:       (none)
 ; MODULE LEVEL
 ;   DECLARATIONS:  DECLARE SUB Mouse (m1%, m2%, m3%, m4%)

.MODEL  MEDIUM
.CODE
        public  mouse

mouse   proc

; Standard entry
        push    bp
        mov     bp,sp

; Get M1% and store it on the stack
        mov     bx,[bp+12]
        mov     ax,[bx]
        push    ax

; Get M2% and store it on the stack
        mov     bx,[bp+10]
        mov     ax,[bx]
        push    ax

; Get M3% into CX register
        mov     bx,[bp+8]
        mov     cx,[bx]

; Get M4% into DX register
        mov     bx,[bp+6]
        mov     dx,[bx]

; Move M2% from stack into BX register
        pop     bx

; Move M1% from stack into AX register
        pop     ax

; Set ES to same as DS (for mouse function 9)
        push    ds
        pop     es

; Do the mouse interrupt
        int     33h
        
; Save BX (M2%) on stack to free register
        push    bx
        
; Return M1% from AX
        mov     bx,[bp+12]
        mov     [bx],ax
        
; Return M2% from stack (was BX)
        pop     ax
        mov     bx,[bp+10]
        mov     [bx],ax
        
; Return M3% from CX
        mov     bx,[bp+8]
        mov     [bx],cx
        
; Return M4% from DX
        mov     bx,[bp+6]
        mov     [bx],dx
        
; Standard exit, assumes four variables passed
        pop     bp
        ret     8

; End of this procedure
mouse   endp
        end
        
