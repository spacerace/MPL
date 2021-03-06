/* add a file to the print spooler que by calling spool(filename,fcb)*/
/* filename is a string containing the name of the file to print*/
/* fcb points to 36 bytes where an fcb can be built */
/* e.g.: main()
         {static char fnam[10]="a:spool.c";
          static char fcb[36];
          spool(fnam,fcb);
         }                    */

spool() /* filename, fcb */
{
#asm
     push si
     push es
     push ds
     pop es
     mov si,[bp+4] ;filename string address
     mov di,[bp+6] ;fcb space
     mov ax,290fh  ;parse filename
     int 21h
     mov dx,di     ;fcb address
     mov ah,0fh    ;open
     int 21h
     mov ah,0      ;add print file to que
     int 2fh       ;spooler queue
     pop es
     pop si
#
}
           