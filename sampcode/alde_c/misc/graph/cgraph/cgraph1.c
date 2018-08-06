/***************************** C _ G R A P H . C ************************/
/*  ....Fast dot and line functions for IBM's EGA adapter in hires      */
/*    (640x350) color mode                                              */
/*    Also double buffering for 256k ega's!!!                           */
/*  This version uses the DeSmet (ver 2.4) C compiler with its built    */      
/* in inline assembler.                                                 */
/*                                                                      */
/* (c)Copywrite 1985 E. Lear - use only for non commercial programming  */
/************************************************************************/

/* Define the interrupt for ega's bios calls */
#define     VIDEO_INT        0x0010

/* Defines for the various modes available from ega's bios              */
/* For more information refer to the Options and Adapters reference     */
/* manual.                                                              */
#define     SET_MODE         0x0000
#define     SET_CUR_TYPE     0x0100
#define     SET_CUR_POS      0x0200
#define     READ_CUR_POS     0x0300
#define     READ_PEN_POS     0x0400
#define     SELECT_PAGE      0x0500
#define     SCROLL_UP        0x0600
#define     SCROLL_DOWN      0x0700
#define     READ_ATC         0x0800
#define     WRITE_ATC        0x0900
#define     WRITE_ATCO       0x0A00
#define     SET_PAL          0x0B00
#define     WRITE_DOT        0x0C00
#define     READ_DOT         0x0D00
#define     WRITE_TTY        0x0E00
#define     READ_MODE        0x0F00
#define     SET_PAL_REG      0x1000
#define     CHAR_GEN         0x1100
#define     SELECT_ALT       0x1200
#define     WRITE_STR        0x1300


unsigned x,y;

/*  Use these values with _doint() for calling IBM's functions and
    interrupts */

extern unsigned _rax, _rbx, _rcx, _rdx, _rsi, _rdi, _res, _rds;
extern void _doint();

/*********************** S E T _ V I D E O _ M O D E **********************/
/* 
         function set_video_mode(mode):

              Inputs:  int - (0..16) = ( mode )

              Returns:  none.

              Example:  set_video_mode(0x10) (Set up 640x350 hi-res mode) 
*/ 


void set_video_mode(mode)
   unsigned mode;
   {
    _rax = SET_MODE | mode;
    _doint(VIDEO_INT);
   }


/*********************** S E T _ P A L _ R E G ************************/
/*
        function set_pal_reg(pal_reg,pal_val)
      
              Inputs: int - (0..15) = ( pal_reg )
                      int - (0..63) = ( pal_val ) 

              Returns: none 

              Example: set_pal_reg(0,1) - (set normally black background
                                           to blue.)
  
*/
void set_pal_reg(pal_reg, pal_val)
   unsigned pal_reg, pal_val;
   {
    _rax = SET_PAL_REG;
    _rbx = (pal_val << 8) | (pal_reg && 0x00ff);
    _doint(VIDEO_INT);
   }

/*********************** B _ W R I T E _ D O T ************************/
/* 
        function b_write_dot(page,row,col,color)

              Inputs: int - (0..1) = ( page )  (In hires mode. i.e 640x350)
                      int - (0..349) = ( row )    "        "        "
                      int - (0..639) = ( col )    "        "        "
                      int - (0..15)  = ( color ) ( In 16 color modes )

              Returns: none.

              Example: b_write_dot(0,20,20,1) - Write a blue dot on page 
                                                one at location 20,20.
*/      

void b_write_dot(page,row,col,color)
   unsigned page,row,col,color;
   {
    _rax = WRITE_DOT | color;
    _rbx = page << 8;
    _rdx = row;
    _rcx = col;
    _doint(VIDEO_INT);
   }

/*  pixel table for the write dot routines ! */
int pixel_table[8] = { 0x4080, 0x1020, 0x0408, 0x0102 };


/**************************** W R I T E _ D O T **********************/
/* this is the 'fastest' version of write_dot (no color but white)   */
/* This is the ultra fast write one color write dot routine          */
/* s/r must be off and all planes must be enabled (white)            */

void write_dot(x,y)
    unsigned x,y;
{
#asm
public write_dot
write_dot:
      mov   ax,[bp+6]
      mov   cx,ax                         ; copy y to cx reg.
      shl   ax,1                          ; 
      shl   ax,1                          ; y = y * 4
      add   cx,ax                         ; y = (y*4 + y) = 5*y
dot_buffer_start:
      add   cx,0a000h                     ; a0 -> ch = base page for ega
      mov   ax,[bp+4]
      and   al,07h                        ; al >= 0 and <= 7
      mov   bx,offset pixel_table_        ; use al as lookup for bit position
      xlat  pixel_table_                  ; al now contains bit to write
      shr   si,1
      shr   si,1
      shr   si,1                          ; si = si / 8
      mov   es,cx                         ; es now = base adr. + y * 5 * 16
      or    es:[si],al
#
   }


/***************************** L I N E _ D R A W ***********************/
/*  This is the ultra fast line drawing routine.  It writes only in    */
/*  white and s/r mode must be turned off                              */

void line_draw(x1,y1,x2,y2)
   unsigned x1,y1,x2,y2;
   {
#asm
      push  ds                         ; save ds reg.
      mov   si,[bp+04h]                ; reg. si = x1
      mov   di,[bp+08h]                ; reg. di = x2
      mov   cx,[bp+06h]                ; reg. cx = y1
      mov   dx,[bp+0ah]                ; reg. dx = y2
      cmp   si,di                      ; cmp x1, x2
      jle   swapxy                     ; skip if (x1<=x2)
      xchg  cx,dx                      ; x1 > x2 so swap y1,y2
      xchg  si,di                      ; and swap x1,x2
swapxy:
      sub   dx,cx                      ; dy = y2-y1
      mov   al,80                      ; deldy = 80
      jge   ydown                      ; skip if y1<=y2
      neg   dx                         ; |y2-y1|
      neg   al                         ; -80
ydown:
      sub   di,si                      ; absdx = x2-x1
      cmp   di,dx                      ; di = absdx, dx = absdy
      lahf                             ; save flags
      jnl   minmax                     ; skip if absdx >= absdy
      xchg  di,dx                      
minmax:
      xchg  ax,bp
; 
; compute initial ds address (a000h + y * 80)
; al = initial pixel
      mov   ax,cx                      ; copy y to ax reg.
      shl   ax,1
      shl   ax,1                       ; y = y * 4
      add   cx,ax                      ; y = y * 4 + y = 5y
      shl   cx,1
      shl   cx,1
      shl   cx,1
      shl   cx,1                       ; y = 5y * 16
      mov   bx,offset pixel_table_     ; get location of pixel lookup table
      mov   al,07h
      and   ax,si                      ; mask out bits > 8 
      xlat  pixel_table_               ; get starting pixel location
      sal   dx,1                       ; dx = delse = dmin * 2
      xchg  ax,dx                      ; ax = delse, dx = pixel location
      xchg  ax,bp                      ; ax = flags(absdx,absdy), bp=delse
      sahf                             ; cmp absdx,absdy
      pushf                            ; save flags on stack
      cbw                              ; ax = deldy 
      mov   cs:delsy+2,ax              ; save deldy
      mov   cs:deldy+2,ax
      mov   cs:deldy2+2,ax
     
      xchg  ax,bp                      ; ax = dmin * 2 = delse
      mov   cs:delse+2,ax              ; save delse
      mov   cs:delse2+2,ax
      sub   ax,di                      ; ax = dmin * 2 - dmax = d
      mov   bp,ax                      ; bp = d = error for start
      sub   ax,di                      ; ax = dmin * 2 - dmax * 2
      mov   cs:delde+2,ax              ; save delde
      mov   cs:delde2+2,ax
      sar   si,1
      sar   si,1
      sar   si,1                       ; x = x / 8
      add   si,cx
line_buffer_start:
      mov   cx,0a000h
      mov   ds,cx
      popf
      mov   cx,di
      jns   delsx2

      or    [si],dl
      or    cx,cx
      jz    lineexit
      or    bp,bp
      jge   diagonal

straight:
delsy:
      add   si,1000h                   ; increment y by -80 or 80
      or    [si],dl                    ; or in byte on screen
      dec   cx                         ; decrement loop count
      jz    lineexit                   ; jump to end if cx = 0
delse:
      add   bp,1000h                   ; add error term
      js    straight

diagonal:
      ror   dl,1                       ; rotate pixel right
      jnc   noincr                     ; skip if dl > 0
      inc   si                         ; increment x 
noincr:
deldy:
      add   si,1000h
      or    [si],dl                    ; write the dot
      dec   cx                         ; decrement loop count
      jz    lineexit
delde:
      add   bp,1000h                   ; add error term
      js    straight                   ; if bp<0 goto straight
      jmp   diagonal                   ; if bp>=0 goto diagonal
;
; absdeltax > absdeltay
;
delsx2:
      or    [si],dl
      jcxz  lineexit
      or    bp,bp
      jge   diagonal2
straight2:
      ror   dl,1                       ; rotate pixel right
      jnc   noincr2
      inc   si
noincr2:
      or    [si],dl                    ; write the dot
      dec   cx
      jz    lineexit
delse2:
      add   bp,1000h
      js    straight2
diagonal2:
      ror   dl,1
      jnc   noincr3
      inc   si
noincr3:
deldy2:
      add   si,1000h
      or    [si],dl
      dec   cx
      jz    lineexit
delde2:
      add   bp,1000h
      js    straight2
      jmp   diagonal2
lineexit:
      pop   ds
      
#      

}

/************************** C _ W R I T E _ D O T *************************/
/* This is the fast version of the color write dot routine.               */

void c_write_dot(x,y)
   unsigned x,y;
   {
#asm
      mov   si,[bp+6]                     ; get y value off stack
      mov   cx,si                         ; copy y to cx reg.
      shl   si,1                          ; 
      shl   si,1                          ; y = y * 4
      add   cx,si                         ; y = (y*4 + y) = 5*y
c_dot_buffer_start:
      add   cx,0a000h                     ; a0 -> ch = base page for ega
      mov   si,[bp+4]                     ; get x value off stack
      mov   dx,03ceh                      ; select bit mask reg. for out
      mov   al,08h                        ; to 03cfh.8
      out   dx,al
      mov   ax,si                         ; copy x to ax reg.
      and   al,07h                        ; al >= 0 and <= 7
      mov   bx,offset pixel_table_        ; use al as lookup for bit position
      xlat  pixel_table_                  ; al now contains bit to write
      shr   si,1
      shr   si,1
      shr   si,1                          ; si = si / 8
      mov   es,cx                         ; es now = base adr. + y * 5 * 16
      inc   dx                            ; mask all but desired pixel
      out   dx,al                         ; output mask to 03fh.8
;
      mov   al,es:[si]                    ; dummy read
      mov   al,0ffh
      mov   es:[si],al                    ; blank pixel
;
#
   }

/*************************** S H O W _ B U F F E R *************************/
/*
         function show_buffer(x)

               Input: x - (0 or 1) Note: this function is provided for double
                                         buffering in 640x350 16 color mode.
                                         You must have 256k in the EGA's
                                         memory for this to work!!!!
               Returns: none
*/
 
void show_buffer(x)
   unsigned x;
   {
#asm
      mov   ax,[bp+4]
      mov   cx,03d4h
      mov   dx,cx                           ; reg cx = 03d4h
      or    ax,ax
      jz    buffer0                         ; Jump if buffer 0 selected
      mov   al,0ch                          ; Start Address High Register
      out   dx,al                           ; Out 3d4,0c
      inc   dx                              
      mov   al,80h
      out   dx,al                           ; Out 3d5,80
      mov   dx,cx                           ; Now fix Address Low Register
      mov   al,0dh
      out   dx,al                           ; Out 3d4,0d
      inc   dx
      mov   al,00h
      out   dx,al                           ; Out 3d5,00 
      jmp   exo
buffer0:         
      mov   al,0ch                          ; Start Address High Register
      out   dx,al                           ; Out 3d4,0c
      inc   dx                              
      xor   al,al
      out   dx,al                           ; Out 3d5,00
      mov   dx,cx                           ; Now fix Address Low Register
      mov   al,0dh
      out   dx,al                           ; Out 3d4,0d
      inc   dx
      xor   al,al
      out   dx,al                           ; Out 3d5,00 
exo:
#
} 


/****************************** S E T _ C O L O R *************************/
/*
         function set_color(color)

            Input: color (0..15) = color Note: this routine must be used
                                               before calling either 
                                               color_write_line or 
                                               color_write_dot. 
            Returns: none
*/

void set_color(color)
   unsigned color;
   {
#asm
      mov   dx,03ceh                   ; Select s/r register
      xor   al,al
      out   dx,al
      inc   dx
      mov   al,[bp+04]                ; output color to s/r resister
      out   dx,al
      dec   dx                         ; dx = 03ce
      mov   al,01                      ; Select s/r register enable
      out   dx,al
      inc   dx
      mov   al,0ffh                    ; select s/r for all planes
      out   dx,al
#
   }

/**************************** C L E A R _ C O L O R ***********************/
/* 
         function clear_color()

            Input: none

            Returns: none       Note:  the function must be called
                                       after using set_color. Otherwise
                                       you will get garbage on the
                                       screen when attempting to write
                                       text with bios.
*/

void clear_color()
   {
#asm
      mov   dx,03ceh
      mov   al,01                      ; Out 03ce,01
      out   dx,al
      inc   dx                         
      dec   al                         ; Disable s/r on all planes
      out   dx,al                      ; Out 03cf,00
      dec   dx
      mov   al,08                      ; Select bit mask register
      out   dx,al                      ; Out 03ce,08
      inc   dx
      mov   al,0ffh                    ; Turn bit mask off
      out   dx,al                      ; Out 03cf,ff
#
}

/************************ C _ L I N E _ D R A W ****************************/      
/* this is the color fast line draw */
void c_line_draw(x1,y1,x2,y2)
   unsigned x1,y1,x2,y2;
   {
#asm
      push  ds                         ; save ds reg.
      mov   si,[bp+04h]                ; reg. si = x1
      mov   di,[bp+08h]                ; reg. di = x2
      mov   cx,[bp+06h]                ; reg. cx = y1
      mov   dx,[bp+0ah]                ; reg. dx = y2
      cmp   si,di                      ; cmp x1, x2
      jle   swapxyc                    ; skip if (x1<=x2)
      xchg  cx,dx                      ; x1 > x2 so swap y1,y2
      xchg  si,di                      ; and swap x1,x2
swapxyc:
      sub   dx,cx                      ; dy = y2-y1
      mov   al,80                      ; deldy = 80
      jge   ydownc                     ; skip if y1<=y2
      neg   dx                         ; |y2-y1|
      neg   al                         ; -80
ydownc:
      sub   di,si                      ; absdx = x2-x1
      cmp   di,dx                      ; di = absdx, dx = absdy
      lahf                             ; save flags
      jnl   minmaxc                    ; skip if absdx >= absdy
      xchg  di,dx                      
minmaxc:
      xchg  ax,bp
; 
; compute initial ds address (a000h + y * 80)
; al = initial pixel
      mov   ax,cx                      ; copy y to ax reg.
      shl   ax,1
      shl   ax,1                       ; y = y * 4
      add   cx,ax                      ; y = y * 4 + y = 5y
      shl   cx,1
      shl   cx,1
      shl   cx,1
      shl   cx,1                       ; y = 5y * 16
      mov   bx,offset pixel_table_     ; get location of pixel lookup table
      mov   al,07h
      and   ax,si                      ; mask out bits > 8 
      xlat  pixel_table_               ; get starting pixel location
      sal   dx,1                       ; dx = delse = dmin * 2
      xchg  ax,dx                      ; ax = delse, dx = pixel location
      xchg  ax,bp                      ; ax = flags(absdx,absdy), bp=delse
      sahf                             ; cmp absdx,absdy
      pushf                            ; save flags on stack
      cbw                              ; ax = deldy 
      mov   cs:delsyc+2,ax             ; save deldy
      mov   cs:deldyc+2,ax
      mov   cs:deldy2c+2,ax
     
      xchg  ax,bp                      ; ax = dmin * 2 = delse
      mov   cs:delsec+2,ax              ; save delse
      mov   cs:delse2c+2,ax
      sub   ax,di                      ; ax = dmin * 2 - dmax = d
      mov   bp,ax                      ; bp = d = error for start
      sub   ax,di                      ; ax = dmin * 2 - dmax * 2
      mov   cs:deldec+2,ax              ; save delde
      mov   cs:delde2c+2,ax
      sar   si,1
      sar   si,1
      sar   si,1                       ; x = x / 8
      add   si,cx
c_line_buffer_start:
      mov   cx,0a000h
      mov   ds,cx
      mov   cx,di
      mov   ah,dl
      mov   al,08                      ; select bit mask register
      mov   dx,03ceh
      out   dx,al   
      inc   dx
      mov   al,ah
      out   dx,al                      ; output bit to mask register
      dec   dx
      mov   al,[si]                    ; dummy read
      mov   al,0ffh
      mov   [si],al                    ; write all ones to address
      or    cx,cx                      ; check for zero line length
      jz    lineexitc
      popf
      jns   delsx2c
      or    bp,bp
      jge   diagonalc
straightc:
delsyc:
      add   si,1000h                   ; increment y by -80 or 80
      mov   al,[si]                    ; dummy read for data latches on ega
      mov   al,0ffh
      mov   [si],al                    ; write all ones to address <- si
      dec   cx                         ; decrement loop count
      jz    lineexitc                  ; jump to end if cx = 0
delsec:
      add   bp,1000h                   ; add error term
      js    straightc
diagonalc:
      ror   ah,1                       ; rotate pixel right
      jnc   noincrc                    ; skip if ah > 0
      inc   si                         ; increment x 
noincrc:
      mov   al,08                      ; store bit in bit mask reg.
      out   dx,al
      mov   al,ah
      inc   dx
      out   dx,al
	  dec	dx	
deldyc:
      add   si,1000h
      mov   al,[si]                    ; dummy read
      mov   al,0ffh
      mov   [si],al                    ; write all ones
      dec   cx                         ; decrement loop count
      jz    lineexitc
deldec:
      add   bp,1000h                   ; add error term
      js    straightc                  ; if bp<0 goto straight
      jmp   diagonalc                  ; if bp>=0 goto diagonal
;
; absdeltax > absdeltay
;
delsx2c:
      or    bp,bp
      jge   diagonal2c
straight2c:
      ror   ah,1                       ; rotate pixel right
      jnc   noincr2c
      inc   si
noincr2c:
      mov   al,08                      ; store bit in bit mask reg.
      out   dx,al
      mov   al,ah
      inc   dx
      out   dx,al
      dec   dx
      mov   al,[si]                    ; dummy read
      mov   al,0ffh
      mov   [si],al                    ; write all ones
      dec   cx
      jz    lineexitc
delse2c:
      add   bp,1000h
      js    straight2c
diagonal2c:
      ror   ah,1
      jnc   noincr3c
      inc   si
noincr3c:
      mov   al,08                      ; store bit in bit mask reg.
      out   dx,al
      mov   al,ah
      inc   dx
      out   dx,al
      dec	dx
deldy2c:
      add   si,1000h                   ; y increment or decrement by 80
      mov   al,[si]                    ; dummy read
      mov   al,0ffh
      mov   [si],ah
      dec   cx
      jz    lineexitc
delde2c:
      add   bp,1000h
      js    straight2c
      jmp   diagonal2c
lineexitc:
      pop   ds                         ; restore data segment
#
}

/********************** S E T _ W R I T E _ B U F F E R ********************/
/*
         function set_write_buffer(x)

               Input: x - (0 or 1) Note: this function is provided for double
                                         buffering in 640x350 16 color mode.
                                         You must have 256k in the EGA's
                                         memory for this to work!!!!
               Returns: none
*/
 
void set_write_buffer(x)
   unsigned x;
   {
#asm
      mov   ax,[bp+4]
      or    ax,ax
      mov   ax,0a000h
      jz    lower_write
      mov   ax,0a800h
lower_write:
      mov   cs:dot_buffer_start+2,ax
      mov   cs:line_buffer_start+1,ax
      mov   cs:c_line_buffer_start+1,ax
      mov   cs:c_dot_buffer_start+2,ax
#
} 

void  write_lower_pattern()
  {
      unsigned int i,j,k=0;
      set_color(3);                  /* Set the color to cyan */
      for(i=150;i<340;i += 3)        /* Draw pattern with cyan lines */
         c_line_draw(1,150,600,i);

      set_color(4);					 /* Set the color to red  */
      for(i=1;i<340;i += 3)          /* Draw pattern with red lines */
         c_line_draw(300,1,100,i);

      set_color(1);                  /* Set the color to blue  */

      for(i=0;i<=340;i += 10)        /* Draw a grid pattern    */
         c_line_draw(1,i,600,i);
      for(i=0;i<=600;i += 10)
         c_line_draw(i,1,i,340);

      for(i=300;i<600;i += 4,k++)    /* Show off the color dot writing */
      {
       set_color(k);  
       for(j=1;j<150;++j)
        {
          c_write_dot(i,j);
        }
      }
  }


void  write_upper_pattern()
  {
      unsigned int i,j,k=0;
      set_color(5);                  /* Set the color to magenta */
      for(i=150;i<340;i += 4)        /* Draw pattern with magenta lines */
         c_line_draw(4,150,600,i);

      set_color(2);					    /* Set the color to green  */
      for(i=1;i<340;i += 2)          /* Draw pattern with red lines */
         c_line_draw(300,1,200,i);

      set_color(12);                  /* Set the color to bright red  */

      for(i=0;i<=340;i += 20)         /* Draw a grid pattern    */
         c_line_draw(1,i,600,i);
      for(i=0;i<=600;i += 20)
         c_line_draw(i,1,i,340);

      for(i=300;i<600;i += 5,k++)    /* Show off the color dot writing */
      {
       set_color(k);  
       for(j=150;j<300;++j)
        {
          c_write_dot(i,j);
        }
      }
  }

void draw_box(size)
   unsigned int size;
   {
      c_line_draw(0,0,size,0);
      c_line_draw(size,0,size,size);
      c_line_draw(size,size,0,size);
      c_line_draw(0,size,0,0);
   }   

/* Here is a little example to show you how to use the above functions ! */

main()
   {
      unsigned int i,h,times, delay;
      set_video_mode(0x10);    /* SET THE HIRES (640X350 - 16 COLOR) */
      set_write_buffer(0);     /* Do all writing to lower buffer */
      show_buffer(0);          /* Display lower buffer */
      write_lower_pattern();   /* Write 'stuff' to lower buffer */

      set_write_buffer(1);     /* Do all writing to upper buffer */
      write_upper_pattern();   /* Write 'stuff' to upper buffer */
      show_buffer(1);          /* Display the upper buffer */
      for(times=0;times<50000;times++);
      show_buffer(0);
      for(times=0;times<2;times++)
       {
         show_buffer(0);
         for(delay=0;delay<40000;++delay);
         show_buffer(1);
         for(delay=0;delay<40000;++delay);
       }
      set_video_mode(0x10);
      show_buffer(0);
      set_write_buffer(0);

      for(h=0;h<20;++h)
       for(i=1;i<16;++i)
        {
         set_color((i+h) & 15);
         if(i % 2)
          {
            show_buffer(0);
            set_write_buffer(1);
          }
         else
          {
            show_buffer(1);
            set_write_buffer(0);
          } 
         for(times=i*20-10;times<i*20;times++)
             draw_box(times);
         }

      show_buffer(0);

      clear_color();                 /* You need this command to     */
                                     /* clean up after your graphics */
                                     /* operations                   */ 
      printf("All done......\n\n");
   }
                                                                                                                   