; ============================================================================
; ============================================================================
; Figure 8: Kernel Module

; ============================================================================

CODE    SEGMENT PARA PUBLIC 'CODE'
ORG     100h
ASSUME  CS:CODE, DS:DATA, ES:NOTHING, SS:STACK

max_proc_entries              EQU        64
                              
pseudo_over_struct            STRUC
      proc_data_segment       DW        ?
      proc_extra_segment      DW        ?
      proc_entry_count        DW        ?
      proc_entry_ptr          DD max_proc_entries DUP (?)
pseudo_over_struct            ENDS

main  PROC  NEAR

      MOV   AX, DATA                       ; Segment initialization
      MOV   DS, AX

check_for_emm_loaded:
      CALL  test_for_EMM                   ; Use the "interrupt vector"
      JE    get_emm_page_frame             ; technique to determine
      JMP   emm_err_exit                   ; whether EMM is loaded

get_emm_page_frame:
      MOV   AH, 41h                        ; Get the page frame base
      INT   67h                            ; address from EMM
      OR    AH, AH
      JZ    allocate_64K
      JMP   emm_err_exit

allocate_64K:
      MOV   exp_mem_segment, BX            ; Allocate 4 pages of expand-
      MOV   AH, 43h                        ; ed memory for this example.
      MOV   BX, 4                          ; More can be allocated de-
      INT   67h                            ; pending on the number of
      OR    AH, AH                         ; overlays to be loaded.
      JZ    map_64K                        ; Actually, in this case,
      JMP   emm_err_exit                   ; only a single page is re-
                                           ; quired because the example
                                           ; pseudo-overlay is extreme-
                                           ; ly small.
map_64K:
      MOV   handle, DX                     ; Map in the first 4 logical
      MOV   CX, 4                          ; pages at physical pages 0
map_pages_loop:                            ; through 3
      MOV   AH, 44h                        ;    logical page 0 at
      MOV   BX, CX                         ;       physical page 0
      DEC   BX                             ;    logical page 1 at
      MOV   AL, BL                         ;       physical page 1
      MOV   DX, handle                     ;    logical page 2 at
      INT   67h                            ;       physical page 2
      OR    AH, AH                         ;    logical page 3 at 
      LOOPE map_pages_loop                 ;       physical page 3
      JE    init_load_struct               ; If additional overlays were
      JMP   emm_err_exit                   ; required, each overlay 
                                           ; would be loaded after map-
                                           ; ping and a new set of
                                           ; logical pages would be
                                           ; mapped at the same
                                           ; physical pages.

init_load_struct:
      MOV   ES, exp_mem_segment            ; Initialize pseudo-overlay
      MOV   DI, 0                          ; environment and procedure
      MOV   CX, (SIZE pseudo_over_struct)  ; pointer area. This struc-
      MOV   AL, 0                          ; ture begins at the page 
      REP   STOSB                          ; frame segment address.

      MOV   AX, (SIZE pseudo_over_struct)  ; Compute the load address
      ADD   AX, 000Fh                      ; within expanded memory for
      AND   AX, 0FFF0h                     ; the overlay. The address is
      MOV   CX, 4                          ; rounded up to the next
      SHR   AX, CL                         ; higher paragraph boundary
      ADD   AX, exp_mem_segment            ; immediately following the
      MOV   parm_block.load_segment, AX    ; pseudo-overlay environment
      MOV   parm_block.reloc_factor, AX    ; & procedure pointer
                                           ; structure. This computa-
                                           ; tion takes into account
                                           ; the maximum number of
                                           ; procedure entry points
                                           ; which the pseudo-overlay
                                           ; is going to return to
                                           ; this program.

      MOV   WORD PTR entry_point[0], 100h  ; Build .COM file entry
      MOV   WORD PTR entry_point[2], AX    ; point

      MOV   AH, 4Bh                        ; Load the pseudo-overlay
      MOV   AL, 03h                        ; using the DOS "load
      LEA   DX, pseudo_over_name           ; overlay" function
      PUSH  DS
      POP   ES
      LEA   BX, parm_block
      INT   21h
      JC    emm_err_exit

      PUSH  DS                             ; Transfer control to the
      PUSH  ES                             ; loaded pseudo-overlays
      CALL  DWORD PTR entry_point          ; initialization code
      POP   ES
      POP   DS
      OR    AH, AH
      JZ    call_over_procedures
      JMP   emm_err_exit

call_over_procedures:
      MOV   ES, exp_mem_segment            ; As an example of passing
      MOV   BX, 0                          ; control to a procedure
      MOV   DI, 0                          ; existing in expanded
      MOV   CX, ES:[BX].proc_entry_count   ; memory, each procedure con-
      JCXZ  deallocate_exp_memory          ; tained in the overlay will
                                           ; be called in sequence.
                                           ; Obviously, a single pro-
                                           ; cedure could be called
                                           ; just as easily.

pseudo_over_call_loop:
      PUSH  BX
      PUSH  CX
      PUSH  DI
      PUSH  ES
      PUSH  DS

      LDS   AX, ES:[BX+DI].proc_entry_ptr
      MOV   WORD PTR CS:tp_ent_ptr[0], AX
      MOV   WORD PTR CS:tp_ent_ptr[2], DS

      MOV   AX, 123                        ; Pass 2 numbers to
      MOV   DX, 23                         ; the procedures

      MOV   DS, ES:[BX].proc_data_segment  ; Set up pseudo-overlays
      MOV   ES, ES:[BX].proc_extra_segment ; segment environment
      CALL  DWORD PTR CS:tp_ent_ptr        ; Call each procedure

      POP   DS
      POP   ES
      POP   DI
      POP   CX
      POP   BX

      ADD   DI, 4                          ; Adjust index to the next
      LOOP  pseudo_over_call_loop          ; procedure (4 bytes long)
                                           ; pointer & loop till all
                                           ; have been called

deallocate_exp_memory:
      MOV   AH, 45h                        ; Return the allocated
      MOV   DX, handle                     ; pages to the expanded
      INT   67h                            ; memory manager
      OR    AH, AH
      JNZ   emm_err_exit

exit:
      MOV   AH, 4Ch                        ; Return a normal exit code
      MOV   AL, 0
      INT   21h

emm_err_exit:
      MOV   AL, AH                         ; Display the fact that
      MOV   AH, 09h                        ; an EMM error occurred
      LEA   DX, emm_err_msg                ; Go to the normal exit
      INT   21h
      JMP   exit

      tp_ent_ptr              DD        ?  ; CS relative far pointer
                                           ; used for transfer to the
main  ENDP                                 ; procedures in the
                                           ; pseudo_overlay


; ============================================================================
; ============================================================================
; Figure 9: Procedure to Test for the Presence of EMM 

; ============================================================================

test_for_EMM    PROC NEAR

       MOV     AX, 3567h              ; Issue "get interrupt vector"
       INT     21h

       MOV     DI, 000Ah              ; Use the SEGMENT in ES
                                      ; returned by DOS, place
                                      ; the "device name field"
                                      ; OFFSET in DI.

       LEA     SI, EMM_device_name    ; Place the OFFSET of the EMM
                                      ; device name string in SI,
                                      ; the SEGMENT is already in DS.

       MOV     CX, 8                  ; Compare the name strings
       CLD                            ; Return the status of the
       REPE    CMPSB                  ; compare in the ZERO flag
       RET

test_for_EMM ENDP

CODE        ENDS

; ============================================================================
; ============================================================================
; Figure 10: Pseudo-overlay Module

; ============================================================================

CODE    SEGMENT PARA PUBLIC 'CODE'
ASSUME  CS:CODE, DS:DATA
ORG     100h

actual_proc_entries         EQU         2

overlay_entry_struct        STRUC
        proc_data_segment   DW          ?
        proc_extra_segment  DW          ?
        proc_entry_count    DW          ?
        proc_entry_ptr      DD          actual_proc_entries DUP (?)
overlay_entry_struct        ENDS

; ============================================================================
; ============================================================================
; Figure 11: Procedure to Identify Overlay

; ============================================================================

command_line_entry_point        PROC        NEAR

        MOV     AX, DATA                    ; Set up local data
        MOV     DS, AX                      ; segment

        LEA     DX, overlay_err_msg         ; Display overlay error
        MOV     AH, 09h                     ; message
        INT     21h                

        MOV     AX, 4C00h                   ; Exit back to DOS
        INT     21h

command_line_entry_point        ENDP

; ============================================================================
; ============================================================================
; Figure 12: Data Segment for the Pseudo-overlay Module

; ============================================================================

DATA  SEGMENT PARA PUBLIC 'DATA'

sum_msg         DB   0Dh, 0Ah, 'Sum of numbers = ', '$'
diff_msg        DB   0Dh, 0Ah, 'Difference of numbers = ', '$'
overlay_err_msg DB   'Overlay cannot be executed via the command line$'
powers_of_ten   DW   10000, 1000, 100, 10, 1

DATA  ENDS

END   command_line_entry_point

; ============================================================================
; ============================================================================
; Figure 13: Pseudo-overlay Data Structure Initialization Procedure

; ============================================================================

initialization  PROC    FAR

MOV  AX, DATA                                        ; Set up a local
MOV  DS, AX                                          ; data segment

MOV  AH, 41h                                         ; Get the page 
INT  67h                                             ; frame segment
OR   AH, AH                                          ; address from EMM
JNZ  error       

MOV  ES, BX                                          ; Create a pointer 
MOV  DI, 0                                           ; to the expanded
                                                     ; memory page frame
                                                     ; segment address

MOV  ES:[DI].proc_data_segment, DS                   ; Return local data 
MOV  ES:[DI].proc_extra_segment, DS                  ; & extra segment 
                                                     ; back to the kernel

MOV  WORD PTR ES:[DI].proc_entry_count, 2            ; Return the number
                                                     ; of local call-
                                                     ; able procedures
                                                     ; back to kernel
MOV  WORD PTR ES:[DI].proc_entry_ptr[0], OFFSET sum  ; Return a far
MOV  WORD PTR ES:[DI].proc_entry_ptr[2], SEG    sum  ; pointer to each
MOV  WORD PTR ES:[DI].proc_entry_ptr[4], OFFSET diff ; local callable
MOV  WORD PTR ES:[DI].proc_entry_ptr[6], SEG    diff ; procedure in the
                                                     ; pseudo-overlay
                                                     ; back to kernel

exit:  MOV   AH, 0                                   ; Set status in AH
                                                     ; = passed
error: RET                                           ; Return status
                                                     ; in AH

initialization  ENDP

; ============================================================================
; ============================================================================
; Figure 14: Procedure to Add AX and DX

; ============================================================================

sum     PROC    FAR

        ADD     AX, DX                 ; Add numbers
        PUSH    AX                     ; Display sum message
        LEA     DX, sum_msg
        MOV     AH, 09h
        INT     21h
        POP     AX
        CALL    display_result         ; Display sum
        RET

sum     ENDP

; ============================================================================
; ============================================================================
; Figure 15: Procedure to Subtract AX and DX

; ============================================================================

diff    PROC    FAR

        SUB     AX, DX               ; Subtract numbers
        PUSH    AX                   ; Display difference message
        LEA     DX, diff_msg
        MOV     AH, 09h
        INT     21h
        POP     AX
        CALL    display_result       ; Display difference
        RET

diff    ENDP

; ============================================================================
; ============================================================================
; Figure 16: Procedure to Display Number in AX in Decimal

; ============================================================================

display_result  PROC    NEAR

        LEA     DI, powers_of_ten
        MOV     CX, 5
display_loop:
        XOR     DX, DX            ; Divide the number passed
        DIV     WORD PTR [DI]     ; in AX by descending powers of ten
        ADD     AL, '0'           ; Convert digit to ASCII

        PUSH    DX                ; Output the digit
        MOV     DL, AL
        MOV     AH, 02h
        INT     21h
        POP     AX

        ADD     DI, 2
        LOOP    display_loop
        RET

display_result        ENDP

; ============================================================================
; ============================================================================
; Figure 17: Data and Stack Segment for the Kernel and the Pseudo-overlay

; ============================================================================

DATA            SEGMENT PARA PUBLIC 'DATA'

emm_err_msg        DB    'EMM error occurred$' ; EMM diagnostic message
pseudo_over_name   DB    'OVERLAY.EXE', 0      ; Name of pseudo-overlay
EMM_device_name    DB    'EMMXXXX0'            ; Standard EMM device name
exp_mem_segment    DW    ?                     ; Temp for expanded
                                               ; memory page frame
                                               ; segment address
handle             DW    ?                     ; Temp for handle allo-
                                               ; cated to the kernel
entry_point  DD          ?                     ; Far pointer to the 
                                               ; entry point for a .COM 
                                               ; file
parm_block_struct  STRUC                       ; Structure definition 
    load_segment   DW    ?                     ; for a "load overlay" 
    reloc_factor   DW    ?                     ; parameter block
parm_block_struct  ENDS
parm_block         parm_block_struct <>        ; The actual parameter
                                               ; block

DATA        ENDS


STACK   SEGMENT PARA STACK 'STACK'
        local_stack     DW 256 DUP ('^^')
STACK   ENDS

END        main
