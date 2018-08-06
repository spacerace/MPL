;       CMACROSX.INC
;
;       This file includes supplemental macros for two macros included
;       in CMACROS.INC: parmCP and parmDP. When these macros are used,
;       CMACROS.INC allocates either 1 or 2 words to the variables
;       associated with these macros, depending on the memory model in
;       use. However, parmCP and parmDP provide no support for automatically
;       adjusting for different memory models--additional program code
;       needs to be written to compensate for this. The loadCP and loadDP
;       macros included in this file can be used to provide additional
;       flexibility for overcoming this limit.

;       For example, "parmDP pointer" will make space (1 word in small
;       and middle models and 2 words in compact, large, and huge models)
;       for the data pointer named "pointer". The statement
;       "loadDP ds,bx,pointer" can then be used to dynamically place the
;       value of "pointer" into DS:BX, depending on the memory model.
;       In small-model programs, this macro would generate the instruction
;       "mov dx,pointer" (it is assumed that DS already has the right
;       segment value); in large-model programs, this macro would generate
;       the statements "mov ds,SEG_pointer" and "mov dx,OFF_pointer".


checkDS macro        segmt
           diffcount = 0
           irp d,<ds,DS,Ds,dS>                  ; Allow for all spellings
              ifdif <segmt>,<d>                 ; of "ds".
                 diffcount = diffcount+1
              endif
           endm
           if diffcount EQ 4
              it_is_DS = 0
           else
              it_is_DS = 1
           endif
        endm

checkES macro        segmt
           diffcount = 0
           irp d,<es,ES,Es,eS>                  ; Allow for all spellings
              ifdif <segmt>,<d>                 ; of "es".
                 diffcount = diffcount+1
              endif
           endm
           if diffcount EQ 4
              it_is_ES = 0
           else
              it_is_ES = 1
           endif
        endm

loadDP  macro        segmt,offst,dptr
           checkDS segmt
           if sizeD                             ; <-- Large data model
              if it_is_DS
                 lds  offst,dptr
              else
                 checkES segmt
                 if it_is_ES
                    les  offst,dptr
                 else
                    mov  offst,OFF_&dptr
                    mov  segmt,SEG_&dptr
                 endif
              endif
           else
              mov  offst,dptr                   ; <-- Small data model
              if it_is_DS EQ 0
                 push ds                        ; If "segmt" is not DS,
                 pop  segmt                     ; move ds to segmt.
              endif
           endif
        endm

loadCP  macro        segmt,offst,cptr
           if sizeC                             ; <-- Large code model
              checkDS segmt
              if it_is_DS
                 lds offst,cptr
              else
                 checkES
                 if it_is_ES
                    les  offst,cptr
                 else
                    mov  segmt,SEG_&cptr
                    mov  offst,OFF_&cptr
                 endif
              endif
           else
              push cs                           ; <-- Small code model
              pop  segmt
              mov  offst,cptr
           endif
        endm
