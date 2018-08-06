        TITLE    SAVINT  Save Interrupts 80-FF
        page     60,132

        include  lattice.mac

        PSEG

_MAIN   proc     near
        _fopen   fname,omode,fp1       ; Open output file
        push     ds
        xor      ax,ax
        mov      ds,ax
        mov      si,0200h
        mov      cx,0200h
        mov      di,offset bufloc
        rep      movsb                 ; Copy Int table to local area
        pop      ds
        mov      ds:word ptr buffer,offset bufloc
        mov      ds:word ptr blksiz,0200h
        mov      ds:word ptr numblks,1
        _fwrite  buffer,blksiz,numblks,fp1
        _fclose  fp1
        ret
_MAIN   endp
        ENDPS

        DSEG
fname   db       'd:save.int',0
omode   db       'wb',0
fp1     dw       0
buffer  dw       0
blksiz  dw       0
numblks dw       0
bufloc  db       512 dup (0CCh)

        ENDDS
        end
