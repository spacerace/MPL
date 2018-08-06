
	Extrn	LibMain:far
_TEXT	SEGMENT BYTE PUBLIC 'CODE'
	ASSUME CS:_TEXT
	PUBLIC LibEntry

LibEntry PROC FAR
	push di		; hInstance
	push ds		; data segment
	push cx		; heap size
	push es
	push si
	call LibMain
	ret
LibEntry ENDP
_TEXT	ENDS
End	LibEntry

