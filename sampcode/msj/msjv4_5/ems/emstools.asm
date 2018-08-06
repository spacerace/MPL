;=============================================================================
;                       Assembly language EMS routines
;                         written by Douglas Boling
;                       (c) 1989 Microsoft Corporation
;=============================================================================

 		.MODEL	small

		.DATA

ems_header      db      "EMMXXXX0",0            ;ASCIIZ EMS driver name
fun25_array	dw	96 dup (0)		;Array for physical page data
fun25_array_end	=	$

ems32up_version	db	0  			;Version number for dispatcher
ems32up_calltbl	dw	offset EMSMapMultiple
		dw	offset EMSMapandJump
		dw	offset EMSMapandCall

		.CODE

		PUBLIC	GetEMSMem
		PUBLIC	SaveEMSMem
		PUBLIC	RestoreEMSMem
		PUBLIC	FreeEMSMem
		PUBLIC	CheckAlias
		PUBLIC	MapbyName
		PUBLIC	EMS32Upgrade

;=============================================================================
;GetEMSMem reserves the amount of Expanded memory requested in BX.
; Entry: BX - amount of memory to reserve
;        SI - Pointer to name to assign to handle in ASCIIZ format.
;             Set SI = -1 for no name.
; Exit:  AL - EMM Version.
;        BX - segment of page frame.
;        CX - Size of page frame.
;        DX - EMS handle.
;=============================================================================
GetEMSMem   	proc
		push	bp
		mov	bp,sp
		push	ax
		push	bx
		push	cx
		push	dx
		push	di
		push	si
		push	es
;-----------------------------------------------------------------------------
;See if EMS driver installed.
;-----------------------------------------------------------------------------
		call	FindEMS			;See if EMM is loaded
		or	ah,ah                   ;Check for error
		jne     getp_error              ; RC <> 0, error
;-----------------------------------------------------------------------------
;Reserve pages.
;-----------------------------------------------------------------------------
		mov	bx,[bp-4]		;Get number of pages to res
		mov	ah,43h			;EMS Allocate pages function
		int	67h			;Call EMS Driver
		or	ah,ah			;Check for error
		jne 	getp_error		;If error, exit.
		mov	[bp-8],dx               ;Save handle.
;-----------------------------------------------------------------------------
;Get version.
;-----------------------------------------------------------------------------
		mov     ah,46h			;Get Expanded Memory version
		int     67h
		or      ah,ah
		jne     getp_error
		mov	[bp-2],al		;Save version
		mov	dl,al
;-----------------------------------------------------------------------------
;See how many physical pages and the number of pages in the page frame.
;-----------------------------------------------------------------------------
		mov	cx,4			;Assume EMM ver 3.2, 4 pages
		mov	bx,cx			;Save number of pages in frame
		cmp	dl,40h			;Check for version 4.0
		jb	getp_continue         	;If before 4.0, use 4 pages
		mov	ax,5801h		;Get number of entries in the
		int	67h			;  map array.
		or	ah,ah			;Check for error
		jne	getp_error
		mov	ax,cx			;Copy number of entries
		sal	ax,1                    ;Mult number of entries by 4
		sal	ax,1
		cmp	ax,offset fun25_array_end - offset fun25_array
		jb	getp_2
		mov	ah,0f2h			;Load array too large RC
		jmp 	getp_exit
getp_2:
                mov	bx,cx			;Copy number of physical pages
		mov	ax,ds			;Set ES = DS
		mov	es,ax
		mov	di,offset fun25_array
		mov	ax,5800h		;Get mappable page array
		int	67h			;Call driver
		or	ah,ah
		jne	getp_error
		xor	ax,ax
getp_loop2:
		cmp	ax,[di+2]		;Search list for physical page
		je	getp_3                  ;  zero. That is the start of
		add	di,4                    ;  the page frame.
		loop	getp_loop2
		mov	ah,0f3h			;Page frame not found in list.
getp_error:
		jmp	getp_exit
getp_3:
		xor	bx,bx			;Zero consecutive page count
getp_loop3:
		inc	bx			;Count the number of
		mov	dx,[di]			;  consecutive segments.
		add	dx,1024			;Add num of paragraphs per seg
		cmp	dx,[di+4]		;Compare with next segment
		jne	getp_continue		;If not consecutive, exit
		add	di,4			;Point to next entry
		loop	getp_loop3
getp_continue:
		mov 	[bp-6],bx		;Put page frame size into
		mov	cx,[bp-4]		;  stack and get num of pages
;-----------------------------------------------------------------------------
;Map pages into page frame.
;-----------------------------------------------------------------------------
		cmp	cx,bx			;See if number of pages larger
		jb	getp_4			;  that the size of the frame.
		mov	cx,bx			;Use the page frame size.
getp_4:
		xor	bx,bx			;Start with logical page 0
		mov	dx,[bp-8]		;Get back handle
getp_loop4:
		mov 	al,bl			;  at physical page 0
		mov	ah,44h			;EMS Map Page function
		int 	67h			;Call EMS Driver
		or	ah,ah			;Check for error
		jne 	getp_exit		;If error, exit.
		inc	bx			;Point to next logical page
		loop	getp_loop4
;-----------------------------------------------------------------------------
;Get page frame segment.
;-----------------------------------------------------------------------------
		mov	ah,41h			;EMS Get page frame segment
		int	67h			;Call EMS Driver
		or	ah,ah			;Check for error
		jne 	getp_exit		;If error, exit.
		mov	[bp-4],bx               ;Save page frame segment.
;-----------------------------------------------------------------------------
;Set the handle name.
;-----------------------------------------------------------------------------
		xor	ax,ax			;Zero return code.
		mov	si,[bp-12]		;Get back name pointer
		cmp	si,-1 			;if -1, no name
		je	getp_exit
		mov	dx,[bp-8]		;Get handle
		mov	ax,5301h		;EMS Set Handle Name.
		int	67h			;Call EMS Driver
		or	ah,ah			;Check for error
		jne 	getp_exit		;If error, exit.
getp_exit:
		mov	[bp-1],ah		;Save return code on stack
		pop	es
		pop	si
		pop	di
		pop	dx
		pop	cx
		pop	bx
		pop	ax
		pop	bp
		ret
GetEMSMem   	endp

;=============================================================================
;SaveEMSMem Saves the context of EMS driver.
; Entry:    CX - Size of save area in bytes.
;        ES:DI - Pointer to save area
; Exit:     AH - Return code
;           DX - Index, Used to identify saved state.
;=============================================================================
SaveEMSMem	proc
		push	bx
		push	cx
		push	di
		mov	ax,4e03h		;Get save area size
		int	67h
		xor	ah,ah			;Clear top byte of save size
		add	ax,4
		mov	bx,di			;Copy buffer pointer
		cmp	word ptr es:[di],4244h	;Check for initialized area
		je	save_1
		mov	es:[di],4244h           ;Load signature
		xor	dx,dx
		mov	es:[di+4],dx            ;Init first index
		dec	dx
		mov	es:[di+2],dx            ;Init queue end pointer
save_1:
;-----------------------------------------------------------------------------
;Find the first free area in the save buffer.
;-----------------------------------------------------------------------------
		inc	di
		inc	di
		add	cx,bx			;Add base address to size but
		sub	cx,ax                   ;  subtract entry size.
		xor	dx,dx
save_2:
		cmp	dx,es:[di+2]		;Find max value of index
		jae	save_3                  ;  in the queue.
		mov	dx,es:[di+2]
save_3:
		cmp	word ptr es:[di],-1     ;If = -1 then this is the
		je	save_4                  ;  last entry.
		add	di,es:[di]		;Point to next entry
		jc	save_areafull		;Check for end of segment and
		cmp	di,cx			;  end of buffer.
		ja	save_areafull
		jmp	short save_2
save_4:
		add	di,4			;Move past queue overhead
		inc	dx			;Create new index.
		push	ax
		mov	ax,4e00h		;Save page map
		int	67h
		or	ah,ah
		pop	ax
		jne	save_exit
		mov	es:[di-4],ax		;Update queue pointers
		mov	es:[di-2],dx
		add	di,ax
		sub	di,4
		mov	word ptr es:[di],-1
save_exit:
		pop	di
		pop	cx
		pop	bx
		ret
save_areafull:
		mov	ah,8ch			;Save area full
		jmp	short save_exit
SaveEMSMem	endp

;=============================================================================
;RestoreEMSMem Restore the context of EMS driver.
; Entry:    DX - Index, value returned by SaveEMSMem when state saved
;        DS:SI - Pointer to save area
; Exit:     AH - Return code
;=============================================================================
RestoreEMSMem	proc
		pushf
		push	cx
		push	di
		push	si
		mov	ax,[si]			;Check for initialized area
		cmp	ax,4244h
		jne	restore_bad
		inc	si			;Point to first entry
		inc	si
restore_1:
		cmp	[si+2],dx               ;Find index value
		je	restore_2
		cmp	word ptr [si],-1	;See if at end of queue
		je	restore_bad
		add	si,[si]
		jc	restore_bad		;If overflow segment, error
		jmp	short restore_1
restore_2:
		mov	di,si			;Copy entry
		add	si,4			;Point to save array
		mov	ax,4e01h		;Restore page map
		int	67h
		or	ah,ah
		jne	restore_exit
		sub	si,4
		add	si,[di]
restore_3:
		cmp	word ptr [si],-1
		je	restore_4
		mov	cx,[di]			;Load size
		cld
		rep	movsb			;Copy next entry
		jmp	short restore_3
restore_4:
		mov	word ptr [di],-1
restore_exit:
		pop	si
		pop	di
		pop	cx
		popf
		ret
restore_bad:
		mov	ah,0a3h
		jmp	short restore_exit
RestoreEMSMem	endp

;=============================================================================
;FreeEMSMem Deallocates EMS memory
; Entry:   DX - handle to deallocate
;=============================================================================
FreeEMSMem	proc
		mov	ah,45h			;Deallocate pages
		int	67h
		ret
FreeEMSMem	endp

;=============================================================================
;CheckAlias Checks for memory aliasing support by EMS driver
; Exit:   AH - return code
;         AL - 1 Aliasing not supported, 0 - Aliasing supported.
;=============================================================================
CheckAlias	proc
		pushf
		push	bx
		push	cx
		push	dx
		push	di
		mov	ah,41h			;Get page frame segment
		int	67h
		or	ah,ah
		jne	chkalias_exit1
		mov	cx,bx			;Copy page frame
;Allocate 1 logical page
		mov	ah,43h			;EMS Allocate page
		mov	bx,1
		int	67h
		or	ah,ah
		jne	chkalias_exit1
;Map logical page to physical pages 0 and 1.
		mov	ax,4400h		;Map to page 0
		mov	bx,0			;Logical page 0
		int	67h
		or	ah,ah
		jne	chkalias_exit
		mov	ax,4401h		;Map to page 1
		mov	bx,0			;Logical page 0
		int	67h
		or	ah,ah
		jne	chkalias_exit
;Write pattern to physical page 0, then compare to page 1.
		xor 	bl,bl 			;Clear alias flag
		push	es
		mov	es,cx
		add	cx,400h
		push	cx
		mov	di,100h			;Point 256 bytes inside page.
		cld
		mov	al,5ah			;Write to page 0 then check
		mov	cx,8                    ;  for aliasing by scanning
		rep	stosb                   ;  for the same pattern on
		mov	cx,8                    ;  page 1.
		pop	es
		mov	di,100h
		repe	scasb
		pop	es
		je 	chkalias_exit
		inc	bl
chkalias_exit:
		mov	ah,45h			;Deallocate pages
		int	67h
		mov	al,bl			;Copy alias flag
chkalias_exit1:
		pop	di
		pop	dx
		pop	cx
		pop	bx
		popf
		ret
chkalias_error:
		mov	ax,8000h		;Indicate SW error
		jmp	short chkalias_exit1
CheckAlias	endp

;=============================================================================
;MapbyName Map expanded memory pages by name.
; Entry:   AL - Physical page
;          BX - Logical page to map.
;       DS:SI - Pointer to Handle name in ASCIIZ format
; Exit:    AH - Return code
;          DX - EMS Handle
;=============================================================================
MapbyName	proc
		push	cx
		mov	cx,ax			;Save physical page number
		mov	ax,5401h		;EMS find handle function
		int	67h			;Call EMS driver
		or	ah,ah			;Check for error
		jne	mbn_exit		;If error, exit
		mov     al,cl			;Get back physical page
		mov	ah,44h                  ;EMS Map page function
		int	67h			;Call EMS driver
		pop	cx
mbn_exit:
		ret
MapbyName	endp

;======================================================================
; FindEMS determines if the EMM is loaded.
;======================================================================
FindEMS		proc
		call	check_ems_hdl		;Check for driver using
		jnc	find_1        		;  open handle technique
		rcl	ah,1         		;If not enough handles, try
		jc 	find_driver_error       ;  interrupt technique.
		call	check_ems_int
		jnc	find_1        		;If manager found, continue
find_driver_error:
		mov 	ah,0f1h			;Set error code.
		jmp	short find_exit
find_1:
		mov     ah,40h			;Check status
		int     67h
find_exit:
		ret
FindEMS		endp

;======================================================================
; Test for EMS driver (Open Handle Technique)
;======================================================================
check_ems_hdl	proc
		mov     dx,offset ems_header
		mov     ax,3d00h		;Open device read only.
		int	21h
		jnc	cemsh_device_open
		cmp	ah,4			;If file or path not found,
		jb	cemsh_not_found		;  no EMS driver.
		mov	ah,01h			;Too many open handles or
		jmp	short cemsh_error	;  access denied.
cemsh_device_open:
		mov	bx,ax			;Copy handle
		mov	ax,4400h		;Get device information.
		int	21h
		jc	cemsh_not_foundclose
		test	dx,80h			;Test file/device bit
		je	cemsh_not_foundclose	;0 = file not device
		mov	ax,4407h		;Get output status
		int	21h
		jc	cemsh_not_foundclose	;If error, no device
		cmp	al,0ffh			;Check for ready status
		jne	cemsh_not_foundclose	;If not ready, no device
		mov	ah,3eh			;Close handle
		int	21h			;BX contains handle
		clc
cemsh_exit:
		ret
cemsh_not_foundclose:
		mov	ah,3eh			;Close handle
		int	21h			;BX contains handle
cemsh_not_found:
		mov	ah,81h			;Not found error code
cemsh_error:
		stc
		jmp     short cemsh_exit
check_ems_hdl	endp
 
;======================================================================
; Test for EMS driver (Get Interrupt technique)
;======================================================================
check_ems_int	proc
		push    es                      ;Test for ems driver
		push    di
		mov     ax,3567h                ;Get EMS vector
		int     21h
		mov     di,0ah                  ;Using the segment from the
		mov     si,offset ems_header    ;  67h vector, look at offset
		mov     cx,8                    ;  0ah. Compare the next 8
		cld                             ;  bytes with the expected
		repe    cmpsb                   ;  EMS header. If  they are
		pop     di                      ;  the same, EMS driver
		pop     es                      ;  found.
		jne     cemsi_error
		clc
cemsi_exit:
		ret
cemsi_error:
		stc
		jmp     short cemsi_exit
check_ems_int	endp


;=============================================================================
;EMS32Upgrade
; Entry: AX - EMS 4.0 compatible function number
;        Other registers configured as needed by the fucntion
; Exit:  AH - Return code
;=============================================================================
EMS32Upgrade	proc	far
		pushf
		push	bp
		mov	bp,sp
		push	ax      		;Save function number
		mov	al,EMS32up_version	;Get version
		or	al,al			;See if initialized
		jne	ems32_2                 ;Yes, version already found
		mov	ah,46h			;EMS Get Version
		int	67h			;Call EMM
		or	ah,ah			;Check for error
		jne	short ems32_exit
		mov	EMS32up_version,al	;Set version
ems32_2:
		cmp	al,40h			;Check for version 4.0
		je	ems32_call_driver
		cmp	al,32h			;Check for version 3.2
		je	ems32_3
		mov	ah,0ffh			;Set bad version error code
		jmp	short ems32_exit
ems32_3:
		mov	ax,[bp-2]		;Get function number
		cmp	ah,4eh			;See if function 1 to 15
		jbe	ems32_call_driver	;If so, call EMM
		push	bx
		mov	bx,offset ems32_return
		push	bx			;Put return address on stack
		mov	bx,ax                   ;Convert function into
		mov	bl,bh                   ;  jump table index.
		xor	bh,bh
		sub	bl,50h
		or	bl,bl
		je	ems32_4
		sub	bl,4
		cmp	bl,2
		jbe	ems32_4
		mov	ah,84h			;Unsupported function code
		jmp	short ems32_exit
ems32_4:
		shl	bx,1
		add	bx,offset ems32up_calltbl
		push	[bx]			;Push subroutine addr on stack
		mov	bx,[bp-4]               ;Get original BX
		retn                            ;Call function
ems32_return:
		add	sp,2			;Pull BX off stack
		jmp	short ems32_exit
ems32_call_driver:
		mov	ax,[bp-2]		;Restore AX
		cmp	ah,55h			;See if map and jump
		jne	ems32_5 		;If so, clean up stack since
		add	sp,2                    ;  EMM will not return to
		mov 	[bp+4],ax		;  this routine.
		mov	ax,[bp+2]		;Use AX and flags values to
		mov	[bp+6],ax               ;  overwrite return address.
		pop	bp
		pop	ax			;Clean off extra flags reg
		pop	ax			;Get back AX
		popf                            ;Restore Flags
ems32_5:
		int	67h			;Call EMM
ems32_exit:
		add	sp,2			;Clear off original AX
		pop	bp
		popf
		ret

;=============================================================================
;EMSMapMultiple  simulates the Map Multiple function of the EMS 4.0 driver.
; Entry: AL - subfunction. 0 = Use physical page numbers
;                          1 = Use physical page segments
;        CX - Size of mapping array
;        DX - EMS Handle
;     DS:SI - Pointer to array of mapping structures (1 structure per page)
; Exit:  AH - Return code
;
;  Page mapping structure:
;          log_to_phys_struc  STRUC
;            log_page_number   dw  ?
;            phy_page_number   dw  ?
;          log_to_phys_struc  ENDS
;=============================================================================
mapm_pfp	equ	[bp-2]			;Pointer to page frame
EMSMapMultiple	proc
		push	bp
		mov	bp,sp
		sub	sp,2			;Create room for local var
		push	bx
		push	cx
		push	di
		push	si
		cmp	al,1			;Check for legal subfunction
		jbe	mapm_1
		mov	ah,8fh			;Subfunction code invalid
		jmp	short mapm_exit
mapm_1:
		xor	ah,ah			;Clear return code.
		or      cx,cx			;See if mapping 0 pages
		je	mapm_exit		;If so, exit.
		xor	ah,ah
		mov	di,ax			;Copy subfunction
		mov	ah,41h			;EMS Get Page Frame segment
		int	67h                     ;Call EMS driver
		or	ah,ah			;Check for error
		jne	mapm_exit
		mov	mapm_pfp,bx		;Save page frame pointer
mapm_loop1:
		mov	ax,[si+2]		;Get physical page
		or 	di,di 			;See if map by segment or num
		je	mapm_4
		sub	ax,mapm_pfp		;Sub starting addr of frame
		jae	mapm_3			;If positive, continue
mapm_2:
		mov	ah,8bh			;Page segment illegal RC
		jmp	short mapm_exit		;Error, goto exit
mapm_3:
		push	dx			;Save handle
		xor	dx,dx			;Clear high word
		mov     bx,400h			;Divide by segment size
		div	bx
		or	dx,dx			;Check for remainder. If found
		pop	dx			;  the segment was not on a
		jne     mapm_2			;  proper boundry.
mapm_4:
		mov	bx,[si]			;Get logical page
		mov	ah,44h			;EMS Map page
		int	67h			;Call EMS driver
		or	ah,ah			;Check for error
		jne	mapm_exit
		add	si,4
		loop	mapm_loop1
mapm_exit:
		pop	si
		pop	di
		pop	cx
		pop	bx
		add	sp,2
		pop	bp
		ret
EMSMapMultiple	endp

;=============================================================================
;EMSMapandJump  Simulates the Map and Jump function of the EMS 4.0 driver.
; Entry: AL - Subfunction. 0 = Use physical page numbers
;                          1 = Use physical page segments
;        DX - EMS Handle
;     DS:SI - Pointer to map and jump structure
; Exit:  AH - Return code
;
;  Map and Jump structure:
;          map_and_jump_struc    STRUC
;            target_addr          dd  ?
;            log_phys_map_len     db  ?
;            log_phys_map_ptr     dd  ?
;          map_and_jump_struc    ENDS
;
;  Page mapping structure:
;          log_phys_map_struc  STRUC
;            log_page_number   dw  ?
;            phy_page_number   dw  ?
;          log_phys_map_struc  ENDS
;=============================================================================
EMSMapandJump	proc
		push	bx
		push	cx
		push	si
		push	ds
		xor	cx,cx			;Clear size
		mov	cl,ds:[si+4]		;Get size of map array
		lds	si,ds:[si+5]		;Get pointer to map array
		call	EMSMapMultiple		;Call MapMultiple to map pages
		or	ah,ah			;Check for error
		pop	ds
		pop	si
		pop	cx
		pop	bx
		jne	mapandjump_exit 	;If so, error code in AH. Exit
		mov	ax,ds:[si]		;Copy jump address over return
		mov	[bp+4],ax		;  address on the stack.
		mov	ax,ds:[si+2]
		mov	[bp+6],ax
		xor	ax,ax			;Zero return code.
mapandjump_exit:
		ret
EMSMapandJump 	endp

;=============================================================================
;EMSMapandCall  Simulates the Map and Call function of the EMS 4.0 driver.
; Entry: AL - Subfunction. 0 = Use physical page numbers
;                          1 = Use physical page segments
;                          2 = Get Stack space size
;        DX - EMS Handle
;        DS:SI - Pointer to map and call structure
; Exit:  AH - Return code
;
;  Map and Call structure:
;          map_and_call_struc    STRUC
;            target_addr          dd  ?
;            new_page_map_len     db  ?
;            new_page_map_ptr     dd  ?
;            old_page_map_len     db  ?
;            old_page_map_ptr     dd  ?
;          map_and_call_struc    ENDS
;
;  Page mapping structure:
;          log_phys_map_struc  STRUC
;            log_page_number   dw  ?
;            phy_page_number   dw  ?
;          log_phys_map_struc  ENDS
;=============================================================================
EMSMapandCall	proc
		push	ax
		mov	ax,4e03h		;Get size of save array
		int 	67h
		or	ah,ah
		pop	bx			;Get back function
		jne	mac_0
		cmp	bl,2			;Check for subfunction 2
		jne	mac_1
		add	al,24			;Add aditional room on stack
mac_0:
		jmp	mac_exit1  		;  on the stack.
mac_1:
		push	si			;Save array pointer
		push	ds
		push	di			;Save registers needed for
		push	es                      ;  save page map call
		sub	sp,ax			;Create room on stack
		mov	di,ss			;Point to save area on stack
		mov	es,di
		mov	di,sp
		push	ax			;Save size of save area
		mov	ax,4e00h		;Save page map
		int	67h			;Call EMM
		or	ah,ah
		je	mac_2
mac_save_error:
		pop	di			;Get save area size
		add	sp,di			;Remove save area from stack
		jmp	short mac_exit
mac_2:
		mov	ax,[bp-2]		;Get original AX for
		push	cx                      ;  subfunction.
		xor	cx,cx			;Clear size
		mov	cl,ds:[si+4]		;Get size of new array
		lds	si,ds:[si+5]		;Get pointer to new map array
		call	EMSMapMultiple		;Call MapMultiple to map pages
		pop	cx
		mov	ds,[bp-10]
		mov	si,[bp-8] 		;Restore registers
		or	ah,ah			;Check for error
		jne	mac_save_error	 	;If so, error code in AH.
		mov	es,[bp-14]
		mov	di,[bp-12]
		mov	ax,[bp+2]		;Get flags off stack
		push	ax
		popf				;Restore flags
                push	bp			;Save my BP
		mov	bp,[bp]			;Restore original BP
		mov	ah,0			;Set return code
		call	dword ptr ds:[si]	;Call user routine
		pop	ax			;Get back BP used by routine
		xchg	bp,ax  			;Put returned BP on stack in
		mov	[bp],ax                 ;  place of original BP
		pushf				;Put all 16 bits of the flags
		pop	ax                      ;  register in AX.
		mov	[bp+2],ax		;Save new flag state on stack
		mov	[bp-12],di		;Save new ES:DI
		mov	[bp-14],es
		mov	di,si			;Save new DS:SI
        	push	ds
		pop	es
		mov	si,ss			;Point to save area on stack
		mov	ds,si
		mov	si,sp
		inc	si			;Move past save area size
		inc	si
		mov	ax,4e01h		;Restore page map
		int	67h
		pop	si                      ;Get size of save area
                add	sp,si			;Remove save area from stack
		push	es
		pop	ds
		mov	si,di
		or	ah,ah
		jne	mac_exit
		push	cx                      ;Save working registers
		push	si
		push	ds
		mov	si,[bp-8]		;Get pointer to structure off
		mov	ds,[bp-10]              ;  the stack
		xor	cx,cx			;Clear size
		mov	ax,[bp-2]		;Restore subfunction
		mov	cl,ds:[si+9]		;Get size of old array
		lds	si,ds:[si+10]		;Get pointer to old map array
		call	EMSMapMultiple		;Call MapMultiple to map pages
		pop	ds
		pop	si
		pop	cx
mac_exit:
		pop	es
		pop	di
		add	sp,4			;Clear off old SI and DS
mac_exit1:
		ret
EMSMapandCall 	endp

EMS32Upgrade	endp

		end
