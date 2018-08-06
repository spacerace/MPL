        TITLE    GETINT  restore Interrupts 80-FF
        page     60,132

        include  lattice.mac

        PSEG

_MAIN   proc     near
        _fopen   fname,imode,fp1       ; Open output file
        mov      ds:word ptr buffer,offset bufloc
        mov      ds:word ptr blksiz,0200h
        mov      ds:word ptr numblks,1
        _fread   buffer,blksiz,numblks,fp1
        push     es
        xor      ax,ax
        mov      es,ax
        mov      si,offset bufloc
        mov      cx,0200h
        mov      di,0200h
        rep      movsb                 ; Copy local area to Int table
        pop      es

        _fclose  fp1
        ret
_MAIN   endp
        ENDPS

        DSEG
fname   db       'd:save.int',0
imode   db       'rb',0
fp1     dw       0
buffer  dw       0
blksiz  dw       0
numblks dw       0
bufloc  db       512 dup (0CCh)

        ENDDS
        end
