        ;************************************************************;
        ;                                                            ;
        ;          Function 4BH: Load and Execute Program            ;
        ;                                                            ;
        ;          int execute(pprogname,pcmdtail)                   ;
        ;              char *pprogname,*pcmdtail;                    ;
        ;                                                            ;
        ;          Returns 0 if program loaded, ran, and             ;
        ;          terminated successfully, otherwise returns        ;
        ;          error code.                                       ;
        ;                                                            ;
        ;************************************************************;

sBegin  data
$cmdlen =       126
$cmd    db      $cmdlen+2 dup (?) ; Make space for command line, plus
                                ; 2 extra bytes for length and
                                ; carriage return.

$fcb    db      0               ; Make dummy FCB.
        db      'dummy   fcb'
        db      0,0,0,0

                                ; Here's the EXEC parameter block:
$epb    dw      0               ; 0 means inherit environment.
        dw      dataOFFSET $cmd ; Pointer to cmd line.
        dw      seg dgroup
        dw      dataOFFSET $fcb ; Pointer to FCB #1.
        dw      seg dgroup
        dw      dataOFFSET $fcb ; Pointer to FCB #2.
        dw      seg dgroup
sEnd    data
sBegin  code

$sp     dw      ?               ; Allocate space in code seg
$ss     dw      ?               ; for saving SS and SP.

Assumes ES,dgroup

cProc   execute,PUBLIC,<ds,si,di>
parmDP  pprogname
parmDP  pcmdtail
cBegin
        mov     cx,$cmdlen      ; Allow command line this long.
        loadDP  ds,si,pcmdtail  ; DS:SI = pointer to cmdtail string.
        mov     ax,seg dgroup:$cmd    ; Set ES = data segment.
        mov     es,ax
        mov     di,dataOFFSET $cmd+1  ; ES:DI = pointer to 2nd byte of
                                      ; our command-line buffer.
copycmd:
        lodsb                   ; Get next character.
        or      al,al           ; Found end of command tail?
        jz      endcopy         ; Exit loop if so.
        stosb                   ; Copy to command buffer.
        loop    copycmd
endcopy:
        mov     al,13
        stosb                   ; Store carriage return at
                                ; end of command.
        neg     cl
        add     cl,$cmdlen      ; CL = length of command tail.
        mov     es:$cmd,cl      ; Store length in command-tail buffer.

        loadDP  ds,dx,pprogname ; DS:DX = pointer to program name.
        mov     bx,dataOFFSET $epb ; ES:BX = pointer to parameter
                                   ; block.

        mov     cs:$ss,ss       ; Save current stack SS:SP (since
        mov     cs:$sp,sp       ; EXEC function destroys stack).
        mov     ax,4b00h        ; Set function code.
        int     21h             ; Ask MS-DOS to load and execute
                                ; program.
        cli                     ; Disable interrupts.
        mov     ss,cs:$ss       ; Restore stack.
        mov     sp,cs:$sp
        sti                     ; Enable interrupts.
        jb      ex_err          ; Branch on error.
        xor     ax,ax           ; Return 0 if no error.
ex_err:
cEnd
sEnd    code



        ;************************************************************;
        ;                                                            ;
        ;   Function 4BH: Load an Overlay Program                    ;
        ;                                                            ;
        ;   int load_overlay(pfilename,loadseg)                      ;
        ;       char *pfilename;                                     ;
        ;       int  loadseg;                                        ;
        ;                                                            ;
        ;   Returns 0 if program has been loaded OK,                 ;
        ;   otherwise returns error code.                            ;
        ;                                                            ;
        ;   To call an overlay function after it has been            ;
        ;   loaded by load_overlay(), you can use                    ;
        ;   a far indirect call:                                     ;
        ;                                                            ;
        ;   1. FTYPE (far *ovlptr)();                                ;
        ;   2. *((unsigned *)&ovlptr + 1) = loadseg;                 ;
        ;   3. *((unsigned *)&ovlptr) = offset;                      ;
        ;   4. (*ovlptr)(arg1,arg2,arg3,...);                        ;
        ;                                                            ;
        ;   Line 1 declares a far pointer to a                       ;
        ;   function with return type FTYPE.                         ;
        ;                                                            ;
        ;   Line 2 stores loadseg into the segment                   ;
        ;   portion (high word) of the far pointer.                  ;
        ;                                                            ;
        ;   Line 3 stores offset into the offset                     ;
        ;   portion (low word) of the far pointer.                   ;
        ;                                                            ;
        ;   Line 4 does a far call to offset                         ;
        ;   bytes into the segment loadseg                           ;
        ;   passing the arguments listed.                            ;
        ;                                                            ;
        ;   To return correctly, the overlay  must end with a far    ;
        ;   return instruction.  If the overlay is                   ;
        ;   written in Microsoft C, this can be done by              ;
        ;   declaring the overlay function with the                  ;
        ;   keyword "far".                                           ;
        ;                                                            ;
        ;************************************************************;

sBegin  data
                                ; The overlay parameter block:
$lob    dw      ?               ; space for load segment;
        dw      ?               ; space for fixup segment.
sEnd    data

sBegin  code

cProc   load_overlay,PUBLIC,<ds,si,di>
parmDP  pfilename
parmW   loadseg
cBegin
        loadDP  ds,dx,pfilename ; DS:DX = pointer to program name.
        mov     ax,seg dgroup:$lob ; Set ES = data segment.
        mov     es,ax
        mov     bx,dataOFFSET $lob ; ES:BX = pointer to parameter
                                   ; block.
        mov     ax,loadseg      ; Get load segment parameter.
        mov     es:[bx],ax      ; Set both the load and fixup
        mov     es:[bx+2],ax    ; segments to that segment.

        mov     cs:$ss,ss       ; Save current stack SS:SP (because
        mov     cs:$sp,sp       ; EXEC function destroys stack).
        mov     ax,4b03h        ; Set function code.
        int     21h             ; Ask MS-DOS to load the overlay.
        cli                     ; Disable interrupts.
        mov     ss,cs:$ss       ; Restore stack.
        mov     sp,cs:$sp
        sti                     ; Enable interrupts.
        jb      lo_err          ; Branch on error.
        xor     ax,ax           ; Return 0 if no error.
lo_err:
cEnd
sEnd    code
