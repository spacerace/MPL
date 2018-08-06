; Figure 11
; ========


NAME start

; Use the DOSSEG directive to insure that the code segment is the 
; first segment in the module. Since this piece of code will be 
; loaded in at the page frame at D000:0000H the map and call use 
; D000:0000H as the entry point.

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
     int    21H

;    Use search for handel to find handle

     mov    si, offset handle_name
     mov    ax, 5401H              ; set function to serch for 
                                   ; named handle
     int    67H                    ; invoke emm
     or ah, ah
     jnz exit

; DX = handle number     
;
;  Set Register for map and call and call EMM with INT 67

     mov     si, offset map_call  
     mov     al, 0                 ; indicate that values are pages
     mov     ah, 56H               ; set function to map and call
     int     67H                   ; invoke emm 
     or ah, ah
     jnz exit
     
     mov     ah, 09                ; set function for dos 
                                   ; display string
     mov     dx, offset exit_msg
     int 21H
exit:
     pop     ds
     ret
start endp
CODE ENDS

data SEGMENT PUBLIC 'DATA'

; EQUATES

cr    equ  0Dh
lf    equ  0AH

; Structures

log_phys_map_struct      STRUC
   log_page_number     DW ? 
   phys_page_number    DW ? 
log_phys_map_struct      ENDS

map_call_struct      STRUC
    target_address      DD ?       ; Pointer to which EMM will  
                                   ; transfer control 
    new_page_map_length DB ?       ; number new pages to be mapped 
                                   ; on call
    new_page_map_ptr    DD ?       ; pointer to array of 
                                   ; log_phys_map_struc
    old_page_map_length DB ?       ; number of pages to mapped on 
                                   ; return
    old_page_map_ptr    DD ?       ; pointer to array of 
                                   ; log_phys_map_struc
    reseved             DW 4 DUP (?)
map_call_struct ENDS

; Data decalrations

new_map  log_phys_map_struct <1,1> ; maping befor call
old_map  log_phys_map_struct <0,0> ; mapping after call

map_call  map_call_struct <0D0004000H,1,new_map,1,old_map>

handle_name db 'mapcall',0         ; handle name is asciz string                                            
enter_msg   db 'Entering Module 1',cr,lf,'$'
exit_msg    db 'Exiting Module 1',cr,lf,'$'

Data  ends

end  start
