v_sync()/*** wait till vertical blanking (CG card) ***/ 
{
#asm
     push dx
     mov dx,3DAh
try: in al,dx    ; video status
     test al,8   ; blanking?
     jz try      ; not set yeti
     pop dx
#
}
                                                                                                                                                                          