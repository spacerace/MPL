; Figure 12
; ========


NAME start
DOSSEG

data SEGMENT PUBLIC 'DATA'
Data  ends

CODE SEGMENT PUBLIC 'CODE'
	ASSUME CS:CODE, DS:DATA
start proc far
     push    ds
     PUSH    dx
     mov     DX,data	           ; get my data seg into ds
     mov     ds,dx
     pop     dx
     mov     ah, 09                ; set function for dos 
                                   ; display string
     mov     dx, offset enter_msg
     int 21H
     mov     ah, 09                ; set function for dos 
                                   ; display string
     mov     dx, offset exit_msg
     int 21H
     pop     ds
     ret
start endp
CODE ENDS

data SEGMENT PUBLIC 'DATA'
cr    equ  0Dh
lf    equ  0AH
enter_msg   db 'Entering Module 2',cr,lf,'$'
exit_msg    db 'Exiting Module 2',cr,lf,'$'

Data  ends

end  start

