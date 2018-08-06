/*;fastwrite.c	 -	a modification of FastWrite.PAS
  ; 				to run under Microsoft C using the MASM Assembler
  ; 				Writes directly to the screen buffer
  ;
  ; 				modified yet again to run under Turbo C
  ; 				by Jim Texter [71426,417]
  ; 				compile using -B compiler switch
  ;
  ;   This routine is based on the FastWrite routine submitted
  ;   to the Borland SIG on CompuServe. It has been tested for
  ;   use with programs written under the Turbo C Compiler,
  ;   version 1.0. I have tested it under the large and hughe
  ;   memory models only.
  ;   In calling the routine from a C program, the value for
  ;   the string 'st' can be supplied by passing a pointer to
  ;   a string or, as is convenient with C, a string literal
  ;   itself (the program will take the address of the 0th element
  ;   of the string).
  ;
  ;   Acknowledgements:
  ;   Brian Foley submitted the Turbo Pascal inline code version
  ;   of this routine to the Borland SIG on CompuServe. David B.
  ;   Rein helped to convert the code, and some final modifications
  ;   were done by Michael D. Most.
  ;
  ;   Questions may be answered by contacting Brian Foley on
  ;   CompuServe [76317,3247], or Mike Most [70446,1244].
  ;
  ;
  ;**********************************************************
  ;*														*
  ;*			usage:	fastwrite(st,row,col,attr)			*
  ;*					 char *st;							*
  ;*					 int row, col, attr;				*
  ;*														*
  ;**********************************************************/


fastwrite (char *st, int row, int col, int attr)
{
			asm  mov	  bx,si 		   /*save SI in BX	  */
			asm  mov	  ax,40h		   /*get video mode 			   */
			asm  mov	  es,ax
			asm  mov	  dl,es:[49h]	   /*DL=Video mode, stored by BIOS */
			asm  mov	  di,row		   /*DI=Row 					   */
			asm  dec	  di			   /*make row 0-24 instead of 1-25 */
			asm  mov	  cl,4			   /*CL=4, CH=0 				   */
			asm  shl	  di,cl 		   /*DI=Row*32					   */
			asm  mov	  ax,di 		   /*store in AX				   */
			asm  shl	  di,1			   /*DI=Row*32					   */
			asm  shl	  di,1			   /*DI=Row*64					   */
			asm  add	  di,ax 		   /*di=row*80					   */
			asm  add	  di,col		   /*Add (Col +1) to di 		   */
			asm  dec	  di			   /*DI=(Row*80) + Column		   */
			asm  shl	  di,1			   /*account for attribute byte    */
			asm  lds	  si,st 		   /*DS:SI points to String 	*/
			asm  mov	  ah,attr		   /*AH = attribute 			   */
			asm  cmp	  dl,7			   /*Mono?? 					   */
			asm  je 	  mono
			asm  mov	  dx,0b800h 	   /*base of video buffer		   */
			asm  mov	  es,dx 		   /*ES = segment for color memory */
			asm  mov	  dx,03dah		   /*6845 port address			   */
getnext:	asm  lodsb					   /*load next char into al 	   */
			asm  or 	  al,al 		   /*test for null character	   */
			asm  jz 	  exit			   /*exit if it's a null           */
			asm  mov	  cx,ax 		   /*store video word in CX 	   */
			asm  cli					   /*no interrupts				   */
waitnoh:	asm  in 	  al,dx 		   /*get 6845 status			   */
			asm  test	  al,8			   /*Vertical retrace?			   */
			asm  jnz	  store 		   /*if so, go					   */
			asm  rcr	  al,1			   /*else, wait for end of		   */
			asm  jc 	  waitnoh		   /*horizontal retrace 		   */
waith:		asm  in 	  al,dx 		   /*get 6845 status again		   */
			asm  rcr	  al,1			   /*wait for horizontal		   */
			asm  jnc	   waith		   /*retrace					   */
store:		asm  mov	   ax,cx		   /*move word back to AX.. 	   */
			asm  stosw					   /*and then to screen 		   */
			asm  sti					   /*allow interrupts			   */
			asm  jmp	  short getnext    /*get next character 		   */
mono:		asm  mov	  dx,0b000h
			asm  mov	  es,dx 		   /*ES = segment for mono memory  */
next:		asm  lodsb					   /*Load next char into AL 	   */
			asm  or 	  al,al 		   /*Test for null char 		   */
			asm  jz 	  exit			   /*Exit if it's a null           */
			asm  stosw					   /*Move video word into place    */
			asm  jmp	  short next	   /*get next character 		   */
exit:		asm  mov	  si,bx 		   /*restore SI from BX 		   */


}






