
;                         Listing 1
;                  C to Assembly Interface
;     Computer Language, Vol. 2, No. 2 (February, 1985), pp. 49-59
;       Modified to remove duplicate leading part, L.P., 5/29/85
;
; SUBROUTINE TO PERFORM DIRECT SCREEN I/O
;
; int ax, bx, cx, dx ;
; char retval [8] ; 
;
;   dspio( ax, bx, cx, dx, retval ) ;
;   dspio( ax ) ;
;   dspio( ax, bx ) ;  
;   dspio( ax, bx, cx ) ;
;   dspio( ax, bx, cx, dx ) ;
;
;    ax - dx VALUES TO BE PLACED IN THE REGISTERS OF THE SAME NAME
;            AS NEEDED BY PARTICULAR VIDEO I/O FUNCTION
;
;    retval, IF SUPPLIED IS A POINTER TO A 4 WORD AREA INTO WHICH THE
;            4 REGISTERS AX - DX ARE COPIED AFTER RETURN FROM INT 10H.
;
DGROUP  GROUP  DATA
DATA    SEGMENT WORD PUBLIC 'DATA'
        ASSUME  DS:DGROUP
DATA    ENDS
;
PGROUP  GROUP  PROG
PROG    SEGMENT  BYTE  PUBLIC 'PROG'
        ASSUME   CS:PGROUP
        PUBLIC   DSPIO
;
RLAYOUT  STRUC        ; Return address space layout; pointed at by retval
RAX     DW    ?       
RBX     DW    ?
RCX     DW    ?
RDX     DW    ?
RLAYOUT ENDS
;
DSPIO   PROC   NEAR
        POP    SI         ; SAVE RETURN ADDRESS
        POP    AX         ; GET PARAMETERS
        POP    BX
        POP    CX
        POP    DX
        POP    DI         ; OPTIONAL POINTER TO 4 WORD AREA
        INT    10H
        CMP    SP,BP      ; BP IS ALWAYS >= OFFSET OF LAST ARGUMENT
        JA     DONE       ; IF NO RETURN POINTER PASSED, DON'T SAVE VALUES
        MOV    [DI].RAX,AX
        MOV    [DI].RBX,BX
        MOV    [DI].RCX,CX
        MOV    [DI].RDX,DX
        JMP    SI
DONE:   PUSH   DI       
        PUSH   DX       ; RESTORE STACK BECAUSE OF THE POSSIBILITY THAT THEY
        PUSH   CX       ; WERE NOT PASSED AS ARGUMENTS
        PUSH   BX
        JMP SI
DSPIO   ENDP
PROG    ENDS
        END
