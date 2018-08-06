        MOV     DX,03FBh        ; base port COM1 (03F8) + LCR (3)
        MOV     AL,080h         ; enable Divisor Latch
        OUT     DX,AL
        MOV     DX,03F8h        ; set for Baud0
        MOV     AX,96           ; set divisor to 1200 BPS
        OUT     DX,AL
        INC     DX              ; to offset 1 for Baud1
        MOV     AL,AH           ; high byte of divisor
        OUT     DX,AL
        MOV     DX,03FBh        ; back to the LCR offset
        MOV     AL,03           ; DLAB = 0, Parity = N, WL = 8
        OUT     DX,AL
        MOV     DX,03F9h        ; offset 1 for IER
        MOV     AL,0Fh          ; enable all ints in 8250
        OUT     DX,AL
        MOV     DX,03FCh        ; COM1 + MCR (4)
        MOV     AL,0Bh          ; OUT2 + RTS + DTR bits
        OUT     DX,AL
CLRGS:
        MOV     DX,03FDh        ; clear LSR
        IN      AL,DX
        MOV     DX,03F8h        ; clear RX reg
        IN      AL,DX
        MOV     DX,03FEh        ; clear MSR
        IN      AL,DX
        MOV     DX,03FAh        ; IID reg
        IN      AL,DX
        IN      AL,DX           ; repeat to be sure
        TEST    AL,1            ; int pending?
        JZ      CLRGS           ; yes, repeat
