;=============================================================================
;                   Sample program for EMS Toolbox Routines
;                          written by Douglas Boling
;                       (c) 1989 Microsoft Corporation
;=============================================================================

 		DOSSEG
 		.MODEL	small
 		.STACK	512

 		.DATA
EMShandle	dw	0			;EMS handle
pageframe_seg	dw	0

active		db	0			;Flag to indicate int9 active
key_code	db	0			;Code of key pressed
cursor_pos	dw	0			;Saved position of cursor

save_index	dw	0
save_area	db	100 dup (0)
save_area_end	=	$

		.CODE
 		EXTRN	GetEMSMem:PROC
 		EXTRN	SaveEMSMem:PROC
 		EXTRN	RestoreEMSMem:PROC

int09h		dd	0			;Saved interrupt 9 vector

;----------------------------------------------------------------------------
;Keyboard Interrupt.  This routine checks for the hotkey then saves or
;  restores the video page as necessary.
;----------------------------------------------------------------------------
kbdint		proc	far
		assume	cs:_text,ds:nothing,es:nothing
		push	ax
		mov	ah,2			;Get keyboard shift state
		int	16h
		cmp	al,08h			;See if ALT key pressed
		jne	goto_kbdbios
		in	al,60h			;Read keyboard port
		cmp	al,13h			;See if 'R' key pressed
		je 	kbd0
		cmp	al,2Eh			;See if 'C' key pressed
		jne	goto_kbdbios
kbd0:
		push	ds
		push	ax
		mov	ax,@data		;Point DS to data segment.
		mov	ds,ax
		pop	ax
		assume	ds:_data
		mov	key_code,al		;Save key value (C or R)
		cmp	active,0
		je 	kbd1
		pop	ds
goto_kbdbios:
		pop	ax
		jmp	cs:int09h
kbd1:
		inc	active
		sti
		push	bx			;Save registers
		push	cx
		push	dx
		push	di
		push	si
		push	es
		mov	di,@data
		mov	es,di              	;Point to save area
		mov	di,offset save_area
		mov	cx,offset save_area_end - offset save_area
		call	SaveEMSMem		;Save state of EMS driver
		or	ah,ah      		;Check for error, if error,
		jne	kbd_exit	        ;  exit.
		mov	save_index,dx
		mov	ax,4400h		;EMS Map page 0
		xor	bx,bx			;Logical page 0
		mov	dx,EMShandle
		int	67h
		or	ah,ah      		;Check for error, if error,
		jne	kbd_restore             ;  exit.

		mov	ah,0fh			;Get display mode
		int	10h			;Check for valid modes 2,3,7
		cmp	al,7
		je 	kbd2
		cmp	al,2
		jb	kbd_restore
		cmp	al,3
		ja	kbd_restore
kbd2:
		push	ax
		mov	ah,3			;Save cursor position
		int	10h
		mov	cursor_pos,dx
		pop	ax

		mov	es,pageframe_seg
		xor	di,di			;Saved scr at offset 0
		cmp	key_code,2eh		;See if capture function
		je	kbd3
		mov	di,1000h		;Save current scr at 1000h
kbd3:
		call	save_scr		;Save current screen
		call	kb_reset		;Reset keyboard controller

		cmp	key_code,2eh		;See if capture function
		je	kbd_restore		;If so, were done.

		push	ds
		mov	ds,pageframe_seg
		xor	si,si
		call	restore_scr		;Display saved screen
		pop	ds
kb4:
		xor	ah,ah			;Get key
		int	16h
		cmp	al,27			;Check for ESC key. If found
		jne	kb4                     ;  exit. Else get another key.

		push	ds              	;Restore original screen
		mov	ds,pageframe_seg
		mov	si,1000h
		call	restore_scr
		pop	ds

kbd_restore:
		mov	si,offset save_area     ;Restore EMS mapping context
		mov	dx,save_index
		call	RestoreEMSMem
		mov	ah,2			;Set cursor position
		mov	dx,cursor_pos
		int	10h
kbd_exit:
		mov	active,0		;Clear active flag
		pop	es
		pop	si
		pop	di
		pop	dx
		pop	cx
		pop	bx
kbd_exit1:
		pop	ds
		pop	ax
		iret
kbdint	endp

;----------------------------------------------------------------------------
;Save screen.
;----------------------------------------------------------------------------
save_scr	proc	near
		stosw                           ;Save video mode
		mov	ax,1234h		;Buffer initialized flag
		stosw
		mov	cx,25			;25 rows
		xor	dx,dx
saves1:
		push	cx
		mov	cx,80			;80 columns
		xor	dl,dl
saves2:
		mov	ah,2			;Set cursor position
		int	10h
		mov	ah,8			;Read char and attribute
		int	10h
		stosw				;Store in EMS buffer
		inc	dl			;Point to next column
		loop	saves2
		pop	cx
		inc	dh			;Point to next row
		loop	saves1
		ret
save_scr        endp

;----------------------------------------------------------------------------
;Restore screen.
;----------------------------------------------------------------------------
restore_scr 	proc	near
		lodsw				;Get video mode
		xor	ah,ah			;Set mode
		cmp	word ptr [si],1234h	;See if initialized
		jne	restore_exit
		int	10h
		inc	si
		inc	si
		mov	cx,25			;25 rows
		xor	dx,dx			;Start at row 0
restores1:
		push	cx
		mov	cx,80			;80 columns
		xor	dl,dl			;Start at column 0
restores2:
		push	cx
		mov	ah,2			;Set cursor position
		int	10h
		lodsw				;Get char and attr from buffer
		mov	bl,ah			;Copy attribute
		mov	cx,1			;Write 1 character
		mov	ah,9			;Write char and attribute
		int	10h
		inc	dl			;Point to next column
		pop	cx
		loop	restores2
		pop	cx
		inc	dh			;Point to next row
		loop	restores1
restore_exit:
		ret
restore_scr     endp

;-----------------------------------------------------------------------------
;KB_RESET resets the keyboard and signals end-of-interrupt to the 8259
;-----------------------------------------------------------------------------
kb_reset	proc near
		assume	cs:_text
		in 	al,61h			;get control port value
		mov	ah,al 			;save it in AH
		or 	al,80h			;set bit 7
		out	61h,al			;send reset value
		mov	al,ah 			;get original value
		out	61h,al			;send to enable keyboard
		cli	      			;suspend interrupts
		mov	al,20h			;get EOI value
		out	20h,al			;send EOI to 8259
		sti	      			;enable interrupts
		ret
kb_reset	endp


;----------------------------------------------------------------------------
;Initialization. Reserve EMS memory and hook into keyboard interrupt.
;----------------------------------------------------------------------------
main:		mov	ax,@data		;Set DS to data segment
		mov	ds,ax

     		mov	bx,1			;Get 1 page
		mov	si,-1
		call	GetEMSMem		;Allocate EMS memory
		or	ah,ah
		jne	ems_error
		mov	EMShandle,dx		;Save EMS handle
		mov	pageframe_seg,bx	;Save EMS page frame

		push	es
		mov	ax,3509h		;Get and Set int 9 vector
		int	21h
		mov	word ptr int09h,bx
		mov	word ptr int09h[2],es
		mov	ax,2509h
		push	ds
		push	cs
		pop	ds
		mov	dx,offset kbdint
 		int	21h
		pop	ds
		pop	es

		mov	dx,ds
		add	dx,10h
		mov	bx,es
		sub     dx,bx
		mov	ax,3100h		;TSR
		int	21h
ems_error:
		mov	al,ah			;Copy EMS return code
		mov	ah,4ch			;terminate with return code
		int	21h

		END	main

