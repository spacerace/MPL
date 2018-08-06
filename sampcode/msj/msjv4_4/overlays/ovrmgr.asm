                .model  LARGE
                extrn   $$INTNO:byte
                extrn   $$MPGSNBASE:word
                extrn   $$MPGSNOVL:byte
                extrn   $$MPOVLLFA:word
                extrn   $$OVLBASE:byte

                extrn   $$MAIN:far
                extrn   _read_overlay_section:far
                extrn   _errputs:far

                .data                        
_pspaddr        dw      ?               ;for relocation with read_overlay_section
old_ovint       dw      ?               ;old overlay interrupt offset
                dw      ?               ;and segment
_executable_name        db      80 dup (0)      ;space for full pathname

cantmap         db      'Can''t map overlay!...exiting',13,10,0
reentry         db      'Already mapping an overlay!...exiting',13,10,0


                .code

                public  $$OVLINIT
                public  _executable_name
                public  _pspaddr
                public  ax_save,bx_save,cx_save,es_save,ret_ip,ret_cs
                public  req_ov,ov_ofs,ov_seg,in_ovly_mgr,ovly_int,return_from_ov

ax_save         dw      ?               ;save area for used registers
bx_save         dw      ?               
cx_save         dw      ?
es_save         dw      ?

ret_ip          dw      ?               ;original return address 
ret_cs          dw      ?               ;(from call to overlaid routine)

req_ov          db      ?               ;requested overlay number
ov_ofs          dw      ?               ;address to call after mapping
ov_seg          dw      ?

in_ovly_mgr     db      0               ;flag to indicate already mapping

$$OVLINIT       label   far
                push    ax
                push    bx
                push    dx
                push    es
                push    ds                      ;need to use ds:dx
                mov     ax,DGROUP
                mov     ds,ax

                mov     ax,es                   ;initialize _pspaddr 
                mov     _pspaddr,ax

                mov     al,[$$INTNO]            ;get interrupt number
                mov     ah,35h                  ;get overlay number interrupt vector
                int     21h     

                mov     old_ovint,bx
                mov     old_ovint+2,es

                mov     bx,cs
                mov     ds,bx
                mov     dx,offset ovly_int      ;new interrupt
                mov     ah,25h
                int     21h                     ;install new int handler
                pop     ds                       
                pop     es
                pop     dx
                pop     bx
                pop     ax
        
                jmp     $$MAIN                  ;go to mainline code 


;**
;** INTERRUPT HANDLER
;**

ovly_int        proc    far
                cmp     cs:in_ovly_mgr,0        ;are we not here already?
                je      ok                      ;right
; Error if we're already in overlay manager!
                mov     ax,offset reentry       ;error message
                push    ds
                push    ax
                call    _errputs
                add     sp,4
                mov     ax,4c41h                ;exit with error code 41h
                int     21h

ok:             mov     cs:in_ovly_mgr,1        ;note that we're here
                mov     cs:ax_save,ax
                mov     cs:bx_save,bx
                mov     cs:cx_save,cx
                mov     ax,es
                mov     cs:es_save,ax

                pop     bx                      ;bx = return ip
                push    bx
                add     bx,3                    ;adjust for extra crap
                mov     cs:[ret_ip],bx          ;save it
                pop     bx                      ;get original retadd back

                pop     ax                      ;return cs
                mov     cs:[ret_cs],ax          ;save it
                mov     es,ax                   ;es:bx -> bytes after INT

                pop     ax                      ;flags, discard

                xor     ah,ah
                mov     al,byte ptr es:[bx]     ;get requested overlay number
                inc     bx                      ;move to next item
                mov     byte ptr cs:req_ov,al   ;save it
                mov     cx,word ptr es:[bx]     ;get offset in overlay segment
                mov     cs:[ov_ofs],cx          ;save it
        
                push    ax                      ;p3 for C function
                mov     bx,ax
                shl     bx,1                    ;* 2
                add     bx,offset $$MPGSNBASE   ;add to base of segment table
                mov     bx,[bx]
                mov     ov_seg,bx
                push    bx                      ;push the segment for p2
                mov     ax,0                    ;p2, low word (0)
                push    ax                      
                call    _read_overlay_section   ;call the pup!
                add     sp,6                    ;get rid of parms
                cmp     ax,0                    ;error return?
                je      no_error                ;no

; Error if can't map this overlay!
                mov     ax,offset cantmap       ;error message
                push    ds
                push    ax
                call    _errputs
                add     sp,4
                mov     ax,4c42h                ;exit with error code 42h
                int     21h

no_error:       push    cs
                mov     ax,offset return_from_ov;trick up return to our code
                push    ax
                mov     ax,seg $$OVLBASE        ;set up call to overlay code
                push    ax
                mov     ax,ov_ofs               
                push    ax
                retf                            ;jump to overlay code...

return_from_ov:                                 ;..and come back here
                mov     ax,ret_cs               ;restore original retadd
                push    ax
                mov     ax,ret_ip
                push    ax
                mov     ax,cs:es_save           ;and original reg values
                mov     es,ax
                mov     ax,cs:ax_save
                mov     bx,cs:bx_save
                mov     cx,cs:cx_save
                mov     cs:in_ovly_mgr,0        ;we're not here anymore

                ret
ovly_int        endp

                end
