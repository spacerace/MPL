;-------------------------------------------------
; SPEAKER.ASM -- Ring 2 routine for ORGAN program
;                (Accesses Intel 8255 Timer chip)
;-------------------------------------------------

               PUBLIC    Speaker
               .286
               .MODEL    MEDIUM, PASCAL

               .DATA
ClockFreq      dd   1193180

               .CODE     SPEAKER_TEXT
Speaker        PROC FAR  Frequency:WORD
               Cli                                ; Disable interrupts

               Mov  BX, Frequency                 ; Get parameter from stack
               Or   BX, BX                        ; Check if it's zero
               Jz   TurnOff                       ; If so, turn off sound

               Mov  AL, 10110110b                 ; Set flags for programming
               Out  43h, AL

               Mov  AX, WORD PTR [ClockFreq]      ; Calculate timer frequency
               Mov  DX, WORD PTR [ClockFreq + 2]
               Div  BX

               Out  42h, AL                       ; Output low byte
               Jmp  $ + 2                         ; Delay

               Mov  AL, AH                        ; Output high byte
               Out  42h, AL
               Jmp  $ + 2                         ; Delay

               In   AL, 61h                       ; Get 8255 bits
               Jmp  $ + 2                         ; Delay

               Or   AL, 00000011b                 ; Set bits for speaker
               Out  61h, AL                       ; Set 8255 bits
               Jmp  Return

TurnOff:       In   AL, 61h                       ; Get 8255 bits
               Jmp  $ + 2                         ; Delay
               And  AL, 11111101b                 ; Set bits for no speaker
               Out  61h, AL                       ; Set 8255 bits

Return:        Sti                                ; Enable interrupts
               Ret
Speaker        ENDP
               END
