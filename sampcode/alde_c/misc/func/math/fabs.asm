EXTRN  FABSMB    : FAR
EXTRN  GFSEG     : FAR
;
; This routine is needed because C does near calls and FABS expects
;   a far. Since we needed the assembler routine we did some other
;   set-up stuff here, instead of in the C routine that calls this.
;
       PUBLIC    CALLFB
CALLFB PROC      NEAR
       PUSH      BP
       MOV       BP,SP
       MOV       BX,[BP+08]     ; Address of command
       PUSH      BX             ; Save command address
       MOV       AX,[BP+06]     ; Length of string
       MOV       WORD PTR [BX],AX    ; Save string length
       MOV       AX,[BP+04]     ; Address of string
       MOV       WORD PTR [BX+02],AX ; Save address of the string
       MOV       AX,[BP+10]     ; Error
       PUSH      AX             ; Save error address
       MOV       AX,[BP+12]     ; Record number
       PUSH      AX             ; Save record number
       MOV       AX,[BP+14]     ; Address of key
       PUSH      AX             ; Save the address of the key
       CALL      FABSMB
       MOV       SP,BP          ; Restore my stack pointer
       POP       BP
       RET
CALLFB ENDP
       PUBLIC    CALLKY
CALLKY PROC      NEAR
       PUSH      BP
       MOV       BP,SP
       PUSH      DS             ; Save the DS
       PUSH      AX             ; Move anything onto stack
       MOV       AX,SP          ; Now point to anything
       PUSH      AX             ; Now point to anything
       CALL      GFSEG          ; Get the segment
       POP       DS             ; Move in key segment
       MOV       SI,[BP+08]     ; Address of source
       MOV       CX,[BP+06]     ; Length of key
       MOV       DI,[BP+04]     ; Address of destination
       CLD                      ; Set auto increment
       REP       MOVSB          ; Move the bytes
       XOR       AX,AX          ; Zero AX
       STOSB                    ; Store 0 in end of string
       POP       DS             ; Restore the DS
       MOV       SP,BP          ; Restore my stack pointer
       POP       BP
       RET
CALLKY ENDP
                                                                                                 BP
       RET
CALLKY ENDP
                                                                                                 BP
       RET
CALLKY ENDP
                                                                                                                                                                                                                                