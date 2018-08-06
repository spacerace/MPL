buffer  db      1               ; Information ID code
        dw      38              ; Length of following buffer
        dw      1               ; Country ID (USA)
        dw      437             ; Code-page number
        dw      0               ; Date format
        db      '$',0,0,0,0     ; Currency symbol
        db      ',',0           ; Thousands separator
        db      '.',0           ; Decimal separator
        db      '-',0           ; Date separator
        db      ':',0           ; Time separator
        db      0               ; Currency format flags
        db      2               ; Digits in currency
        db      0               ; Time format
        dd      026ah:176ch     ; Monocase routine entry point
        db      ',',0           ; Data list separator
        db      10 dup (0)      ; Reserved
