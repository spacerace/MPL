; |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
; ||                                                                         ||
; ||    SRC FILE :  ISR-CLCK.ASM      [ ver 1.01 @ 85/04/01 jmf ]            ||
; ||                                                                         ||
; ||    FUNCTION :  void set_clck() ; Load new interrupt vector              ||
; ||                void isr_clck() ; Generic clock interrupt 0x1c service   ||
; ||                void rst_clck() ; Restore original vector                ||
; ||                                                                         ||
; ||    COMMENTS :  This module includes the Lattice-C  DOS.MAC interface,   ||
; ||                which is created as follows ...                          ||
; ||                                                                         ||
; ||                  if 'd' model, then rename dm8086.mac dos.mac           ||
; ||                  if 'l' model, then rename lm8086.mac dos.mac           ||
; ||                  if 'p' model, then rename pm8086.mac dos.mac           ||
; ||                  if 's' model, then rename sm8086.mac dos.mac           ||
; ||                                                                         ||
; ||    CAUTIONS :  The original interrupt vector must be restored using     ||
; ||                the rst_clck() function prior to returning to dos.       ||
; ||                                                                         ||
; |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

  INCLUDE       DOS.MAC                         ;  lattice-c interface

; =============================================================================

  INACTIVE      EQU     00000000B               ;  down-counter inactive
  TIMEOUT0      EQU     00000001B               ;  timer_00 time out flag bit
  TIMEOUT1      EQU     00000010B               ;  timer_01 time out flag bit
  TIMEOUT2      EQU     00000100B               ;  timer_02 time out flag bit
  TIMEOUT3      EQU     00001000B               ;  timer_03 time out flag bit
  TIMEOUT4      EQU     00010000B               ;  timer_04 time out flag bit
  TIMEOUT5      EQU     00100000B               ;  timer_05 time out flag bit
  TIMEOUT6      EQU     01000000B               ;  timer_06 time out flag bit
  TIMEOUT7      EQU     10000000B               ;  timer_07 time out flag bit

; ( cont. )( cont. )

; =============================================================================

                DSEG                            ;  lattice-c segment macro

; -----------------------------------------------------------------------------
; The clock interrupt service routine will support eight (8) timers, designated
; timer_00 thru timer_07.  Each is seperately declared as an unsigned, public
; variable.  A pointer to these eight variables, designated timer, is also de-
; fined.  This permits the user to view these variables as individual elements,
; as elements of the timer[] array, or as members of the timer structure.
; -----------------------------------------------------------------------------

  TIMER_00      DW      0                       ;  clock timer-counter # 0
                PUBLIC  TIMER_00                ;        declared public
  TIMER_01      DW      0                       ;  clock timer-counter # 1
                PUBLIC  TIMER_01                ;        declared public
  TIMER_02      DW      0                       ;  clock timer-counter # 2
                PUBLIC  TIMER_02                ;        declared public
  TIMER_03      DW      0                       ;  clock timer-counter # 3
                PUBLIC  TIMER_03                ;        declared public
  TIMER_04      DW      0                       ;  clock timer-counter # 4
                PUBLIC  TIMER_04                ;        declared public
  TIMER_05      DW      0                       ;  clock timer-counter # 5
                PUBLIC  TIMER_05                ;        declared public
  TIMER_06      DW      0                       ;  clock timer-counter # 6
                PUBLIC  TIMER_06                ;        declared public
  TIMER_07      DW      0                       ;  clock timer-counter # 7
                PUBLIC  TIMER_07                ;        declared public
; ----------------------------------------------
                IF      LDATA                   ;  if large data model
  TIMER         DD      TIMER_00                ;     then 32-bit pointer
                ELSE                            ;  if small data model
  TIMER         DW      TIMER_00                ;     then 16-bit pointer
                ENDIF                           ;  end conditional
;               --------------------------------
                PUBLIC  TIMER                   ;  pointer declared public

; -----------------------------------------------------------------------------
; In the event that one of the timer / down-counters decrements to zero, the
; appropriate bit will be set in the public variable clck_flg.
; -----------------------------------------------------------------------------

  CLCK_FLG      DW      0                       ;  bit-mapped time-out flag
                PUBLIC  CLCK_FLG                ;        declared public

; -----------------------------------------------------------------------------
; Before loading the clock interrupt service routine, we must get and save the
; original vector so it can be restored prior to exiting to dos.  Therefore ...
; -----------------------------------------------------------------------------

  OLD1C_IV      LABEL   DWORD                   ;  32-bit interrupt vector
  OLD1C_IP      DW      ?                       ;     instruction pointer
  OLD1C_CS      DW      ?                       ;     code segment

; =============================================================================

                ENDDS                           ;  end segment macro

; ( cont. )( cont. )

                PSEG                            ;  lattice-c segment macro

; |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
; ||                                                                         ||
; ||    FUNCTION :  void set_clck() ; Load new interrupt vector              ||
; ||                                                                         ||
; |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

                IF      LPROG                   ;  if large program model
  SET_CLCK      PROC    FAR                     ;     define as far procedure
                ELSE                            ;  otherwise
  SET_CLCK      PROC    NEAR                    ;     define as near procedure
                ENDIF                           ;  end conditional
                PUBLIC  SET_CLCK                ;  declare public
; ----------------------------------------------
                PUSH    ES                      ;  protect context
                PUSH    DS                      ;     ...
                MOV     AX, DATA                ;  ensure proper segment
                MOV     DS, AX                  ;     ...
; ----------------------------------------------
                MOV     AX, 351CH               ;  get original vector
                INT     021H                    ;      via dos service
                MOV     OLD1C_IP, BX            ;  save offset
                MOV     OLD1C_CS, ES            ;  and code segment
                MOV     DX, OFFSET ISR_CLCK     ;  get new vector offset
                PUSH    CS                      ;  and code segment
                POP     DS                      ;     ...
                MOV     AX, 251CH               ;  set new clock vector
                INT     021H                    ;      via dos service
                POP     DS                      ;  restore context
                POP     ES                      ;     ...
                RET                             ;  return to caller
; ----------------------------------------------
  SET_CLCK      ENDP                            ;  end procedure

; ( cont. )( cont. )

; |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
; ||                                                                         ||
; ||    FUNCTION :  void isr_clck() ; Generic clock interrupt 0x1c service   ||
; ||                                                                         ||
; ||    COMMENTS :  Interrupt 0x1c is invoked by the IBM-BIOS approximately  ||
; ||                18.2 times per second.  The following service monitors   ||
; ||                8 unsigned timer / down-counters and communicates to     ||
; ||                the user via the bit-mapped clck_flg as follows ...      ||
; ||                                                                         ||
; ||                         for each timer-n, 0 <= n <= 7                   ||
; ||                           if timer-n is inactive ( i.e., 0 )            ||
; ||                              continue                                   ||
; ||                           otherwise                                     ||
; ||                              decrement timer-n                          ||
; ||                              if timer-n has timed-out                   ||
; ||                                 set bit-n of clck_flg                   ||
; ||                                                                         ||
; |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

  ISR_CLCK      PROC    FAR                     ;  clock interrupt service
                PUBLIC  ISR_CLCK                ;  declared public
; ----------------------------------------------
                STI                             ;  enable interrupts
                PUSH    AX                      ;  protect context
                PUSH    DS                      ;     ...
                MOV     AX, DATA                ;  ensure proper segment
                MOV     DS, AX                  ;     ...
; ----------------------------------------------
  CHECK_00:     CMP     TIMER_00, INACTIVE      ;  is timer inactive?
                JE      CHECK_01                ;  y: test next timer
                DEC     TIMER_00                ;  n: decrement timer
                JNZ     CHECK_01                ;     if not timeout, cont.
                OR      CLCK_FLG, TIMEOUT0      ;        else flag timeout
; ----------------------------------------------
  CHECK_01:     CMP     TIMER_01, INACTIVE      ;  is timer inactive?
                JE      CHECK_02                ;  y: test next timer
                DEC     TIMER_01                ;  n: decrement timer
                JNZ     CHECK_02                ;     if not timeout, cont.
                OR      CLCK_FLG, TIMEOUT1      ;        else flag timeout
; ----------------------------------------------
  CHECK_02:     CMP     TIMER_02, INACTIVE      ;  is timer inactive?
                JE      CHECK_03                ;  y: test next timer
                DEC     TIMER_02                ;  n: decrement timer
                JNZ     CHECK_03                ;     if not timeout, cont.
                OR      CLCK_FLG, TIMEOUT2      ;        else flag timeout
; ----------------------------------------------
  CHECK_03:     CMP     TIMER_03, INACTIVE      ;  is timer inactive?
                JE      CHECK_04                ;  y: test next timer
                DEC     TIMER_03                ;  n: decrement timer
                JNZ     CHECK_04                ;     if not timeout, cont.
                OR      CLCK_FLG, TIMEOUT3      ;        else flag timeout

; ( cont. )( cont. )

; ----------------------------------------------
  CHECK_04:     CMP     TIMER_04, INACTIVE      ;  is timer inactive?
                JE      CHECK_05                ;  y: test next timer
                DEC     TIMER_04                ;  n: decrement timer
                JNZ     CHECK_05                ;     if not timeout, cont.
                OR      CLCK_FLG, TIMEOUT4      ;        else flag timeout
; ----------------------------------------------
  CHECK_05:     CMP     TIMER_05, INACTIVE      ;  is timer inactive?
                JE      CHECK_06                ;  y: test next timer
                DEC     TIMER_05                ;  n: decrement timer
                JNZ     CHECK_06                ;     if not timeout, cont.
                OR      CLCK_FLG, TIMEOUT5      ;        else flag timeout
; ----------------------------------------------
  CHECK_06:     CMP     TIMER_06, INACTIVE      ;  is timer inactive?
                JE      CHECK_07                ;  y: test next timer
                DEC     TIMER_06                ;  n: decrement timer
                JNZ     CHECK_07                ;     if not timeout, cont.
                OR      CLCK_FLG, TIMEOUT6      ;        else flag timeout
; ----------------------------------------------
  CHECK_07:     CMP     TIMER_07, INACTIVE      ;  is timer inactive?
                JE      END_CLCK                ;  y: done
                DEC     TIMER_07                ;  n: decrement timer
                JNZ     END_CLCK                ;     if not timeout, done
                OR      CLCK_FLG, TIMEOUT7      ;        else flag timeout
; ----------------------------------------------
  END_CLCK:     POP     DS                      ;  restore context
                POP     AX                      ;     ...
                IRET                            ;  return from interrupt
; ----------------------------------------------
  ISR_CLCK      ENDP                            ;  end procedure

; ( cont. )( cont. )

; |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
; ||                                                                         ||
; ||    FUNCTION :  void rst_clck() ; Restore original vector                ||
; ||                                                                         ||
; |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

                IF      LPROG                   ;  if large program model
  RST_CLCK      PROC    FAR                     ;     define as far procedure
                ELSE                            ;  otherwise
  RST_CLCK      PROC    NEAR                    ;     define as near procedure
                ENDIF                           ;  end conditional
                PUBLIC  RST_CLCK                ;  declare public
; ----------------------------------------------
                PUSH    DS                      ;  protect context
                MOV     AX, DATA                ;  ensure proper segment
                MOV     DS, AX                  ;     ...
; ----------------------------------------------
                LDS     DX, OLD1C_IV            ;  load original vector
                MOV     AX, 251CH               ;  set the clock vector
                INT     021H                    ;      via dos service
; ----------------------------------------------
                POP     DS                      ;  restore context
                RET                             ;  return to caller
; ----------------------------------------------
  RST_CLCK      ENDP                            ;  end procedure

; =============================================================================

                ENDPS                           ;  end segment macro
                END                             ;  end of file

                                                                                                                                