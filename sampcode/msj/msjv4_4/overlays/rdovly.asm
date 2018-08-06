;	Static Name Aliases
;
	TITLE   rdovly.c
	NAME    rdovly

	.8087
RDOVLY_TEXT	SEGMENT  WORD PUBLIC 'CODE'
RDOVLY_TEXT	ENDS
_DATA	SEGMENT  WORD PUBLIC 'DATA'
_DATA	ENDS
CONST	SEGMENT  WORD PUBLIC 'CONST'
CONST	ENDS
_BSS	SEGMENT  WORD PUBLIC 'BSS'
_BSS	ENDS
DGROUP	GROUP	CONST, _BSS, _DATA
	ASSUME  CS: RDOVLY_TEXT, DS: DGROUP, SS: DGROUP
EXTRN	_myread:FAR
EXTRN	_myopen:FAR
EXTRN	_myclose:FAR
EXTRN	_errputs:FAR
EXTRN	_mylseek:FAR
EXTRN	_executable_name:BYTE
EXTRN	_pspaddr:WORD
_DATA      SEGMENT
$SG156	DB	'Something''s wrong...can''t read .EXE header',  0dH,  0aH,  00H
$SG159	DB	'Found non-EXE signature!',  0dH,  0aH,  00H
_DATA      ENDS
RDOVLY_TEXT      SEGMENT
	ASSUME	CS: RDOVLY_TEXT
;#include "exehdr.h"
;
;/* externally-defined full pathname of executable */
;
;#define min(a,b) ((a) < (b)) ? (a) : (b)
;#define max(a,b) ((a) < (b)) ? (a) : (b)
;#define OPENMODE_RO 0
;#define MK_FP(a,b) ((void far *)( ((unsigned long)(a) << 16) | (unsigned)(b) ))
;
;extern char executable_name[];
;extern int pspaddr;
;
;void errputs(char far *s);
;long mylseek(int handle, long offset, int type);
;int myread(int handle, void far *buf, int len);
;int myopen(char far *name, int mode);
;int myclose(int handle);
;
;
;int read_overlay_section(
;void far *ovarea,                   /* buffer into which to read him */
;int requested_overlay_number        /* number of overlay to read */
;)
;
;#define RELOC_BUF_SIZE 32
;{
	PUBLIC	_read_overlay_section
_read_overlay_section	PROC FAR
	push	bp
	mov	bp,sp
	sub	sp,190
	push	di
	push	si
;	ovarea = 6
;	requested_overlay_number = 10
;	reloc_buf = -140
;	reloc_ptr = -146
;	reloc_chunk = -186
;	target_ptr = -154
;	eh = -184
;	i = -142
;	startpos = -8
;	nextpos = -12
;	filesize = -150
;	sectionsize = -4
;	imagesize = -190
;	executable_handle = -156
;    struct reloc_entry_ reloc_buf[RELOC_BUF_SIZE];
;    struct reloc_entry_ *reloc_ptr;
;    int reloc_chunk;
;    unsigned far *target_ptr;
;    struct exehdr_ eh;
;    int i;
;    long startpos, nextpos, filesize;
;    long sectionsize, imagesize;
;    int executable_handle;
;
;    /* open executable */
;    executable_handle = myopen(executable_name,OPENMODE_RO);
	mov	ax,0
	push	ax
	mov	ax,OFFSET _executable_name
	mov	dx,SEG _executable_name
	push	dx
	push	ax
	call	FAR PTR _myopen
	add	sp,6
	mov	WORD PTR [bp-156],ax	;executable_handle
;
;    /* determine file size */
;    filesize = mylseek(executable_handle,0L,2);
	mov	ax,2
	push	ax
	sub	ax,ax
	push	ax
	push	ax
	push	WORD PTR [bp-156]	;executable_handle
	call	FAR PTR _mylseek
	add	sp,8
	mov	WORD PTR [bp-150],ax	;filesize
	mov	WORD PTR [bp-148],dx
;
;    /* search from beginning of file */
;    mylseek(executable_handle,0L,0);
	mov	ax,0
	push	ax
	sub	ax,ax
	push	ax
	push	ax
	push	WORD PTR [bp-156]	;executable_handle
	call	FAR PTR _mylseek
	add	sp,8
;
;    while (1) {
$FC153:
;        /* use mylseek() to avoid calling runtime functions */
;        startpos = mylseek(executable_handle,0L,1);
	mov	ax,1
	push	ax
	sub	ax,ax
	push	ax
	push	ax
	push	WORD PTR [bp-156]	;executable_handle
	call	FAR PTR _mylseek
	add	sp,8
	mov	WORD PTR [bp-8],ax	;startpos
	mov	WORD PTR [bp-6],dx
;        if (myread(executable_handle,&eh,sizeof(eh)) != sizeof(eh)) {
	mov	ax,28
	push	ax
	lea	ax,WORD PTR [bp-184]	;eh
	push	ss
	push	ax
	push	WORD PTR [bp-156]	;executable_handle
	call	FAR PTR _myread
	add	sp,8
	cmp	ax,28
	jne	$JCC132
	jmp	$I155
$JCC132:
;            errputs("Something's wrong...can't read .EXE header\r\n");
	mov	ax,OFFSET DGROUP:$SG156
	push	ds
	push	ax
	call	FAR PTR _errputs
	add	sp,4
;            myclose(executable_handle);
	push	WORD PTR [bp-156]	;executable_handle
	call	FAR PTR _myclose
	add	sp,2
;            return(1);
	mov	ax,1
	jmp	$EX139
;	    }
;
;	    if (eh.M_sig != 'M' || eh.Z_sig != 'Z') {
$I155:
	cmp	BYTE PTR [bp-184],77	;eh
	je	$JCC173
	jmp	$I158
$JCC173:
	cmp	BYTE PTR [bp-183],90
	jne	$JCC183
	jmp	$I157
$JCC183:
$I158:
;	        errputs("Found non-EXE signature!\r\n");
	mov	ax,OFFSET DGROUP:$SG159
	push	ds
	push	ax
	call	FAR PTR _errputs
	add	sp,4
;            myclose(executable_handle);
	push	WORD PTR [bp-156]	;executable_handle
	call	FAR PTR _myclose
	add	sp,2
;	        return(1);
	mov	ax,1
	jmp	$EX139
;	    }
;        if (eh.remain_len == 0)
$I157:
	cmp	WORD PTR [bp-182],0
	je	$JCC224
	jmp	$I160
$JCC224:
;            sectionsize = (long)eh.page_len * 512;
	mov	ax,WORD PTR [bp-180]
	sub	dx,dx
	shl	ax,1
	rcl	dx,1
	mov	dh,dl
	mov	dl,ah
	mov	ah,al
	sub	al,al
	mov	WORD PTR [bp-4],ax	;sectionsize
	mov	WORD PTR [bp-2],dx
;        else
	jmp	$I161
$I160:
;            sectionsize = (long)(eh.page_len - 1) * 512 + eh.remain_len;
	mov	ax,WORD PTR [bp-180]
	dec	ax
	sub	dx,dx
	shl	ax,1
	rcl	dx,1
	mov	dh,dl
	mov	dl,ah
	mov	ah,al
	sub	al,al
	add	ax,WORD PTR [bp-182]
	adc	dx,0
	mov	WORD PTR [bp-4],ax	;sectionsize
	mov	WORD PTR [bp-2],dx
$I161:
;
;        if (eh.overlay_number == requested_overlay_number) {
	mov	al,BYTE PTR [bp-158]
	sub	ah,ah
	cmp	ax,WORD PTR [bp+10]	;requested_overlay_number
	je	$JCC297
	jmp	$I162
$JCC297:
;            /* found ours...load and fix up */
;
;            /* move to executable image */
;            mylseek(executable_handle, startpos + eh.hsize * 16, 0);
	mov	ax,0
	push	ax
	mov	ax,WORD PTR [bp-176]
	mov	cl,4
	shl	ax,cl
	sub	dx,dx
	add	ax,WORD PTR [bp-8]	;startpos
	adc	dx,WORD PTR [bp-6]
	push	dx
	push	ax
	push	WORD PTR [bp-156]	;executable_handle
	call	FAR PTR _mylseek
	add	sp,8
;
;            myread(executable_handle, ovarea, (int)(sectionsize - eh.hsize * 16));
	mov	ax,WORD PTR [bp-4]	;sectionsize
	mov	dx,WORD PTR [bp-176]
	mov	cl,4
	shl	dx,cl
	sub	ax,dx
	push	ax
	push	WORD PTR [bp+8]
	push	WORD PTR [bp+6]	;ovarea
	push	WORD PTR [bp-156]	;executable_handle
	call	FAR PTR _myread
	add	sp,8
;
;            /* fix up relocations in the loaded overlay */
;
;            mylseek(executable_handle,startpos + (long)eh.first_reloc,0);
	mov	ax,0
	push	ax
	mov	ax,WORD PTR [bp-160]
	sub	dx,dx
	add	ax,WORD PTR [bp-8]	;startpos
	adc	dx,WORD PTR [bp-6]
	push	dx
	push	ax
	push	WORD PTR [bp-156]	;executable_handle
	call	FAR PTR _mylseek
	add	sp,8
;            while (eh.num_relocs) {
$FC164:
	cmp	WORD PTR [bp-178],0
	jne	$JCC403
	jmp	$FB165
$JCC403:
;                reloc_chunk = min(RELOC_BUF_SIZE,eh.num_relocs);
	mov	ax,WORD PTR [bp-178]
	sub	ax,32
	sbb	cx,cx
	and	ax,cx
	add	ax,32
	mov	WORD PTR [bp-186],ax	;reloc_chunk
;                myread(executable_handle,reloc_buf,reloc_chunk * sizeof(struct reloc_entry_));
	mov	ax,WORD PTR [bp-186]	;reloc_chunk
	shl	ax,1
	shl	ax,1
	push	ax
	lea	ax,WORD PTR [bp-140]	;reloc_buf
	push	ss
	push	ax
	push	WORD PTR [bp-156]	;executable_handle
	call	FAR PTR _myread
	add	sp,8
;                eh.num_relocs -= reloc_chunk;
	mov	ax,WORD PTR [bp-186]	;reloc_chunk
	sub	WORD PTR [bp-178],ax
;                for (i = 0; i < reloc_chunk; i++) {
	mov	WORD PTR [bp-142],0	;i
	jmp	$F166
$FC167:
	inc	WORD PTR [bp-142]	;i
$F166:
	mov	ax,WORD PTR [bp-186]	;reloc_chunk
	cmp	WORD PTR [bp-142],ax	;i
	jl	$JCC482
	jmp	$FB168
$JCC482:
;                    reloc_ptr = reloc_buf + i;
	mov	si,WORD PTR [bp-142]	;i
	shl	si,1
	shl	si,1
	lea	ax,[bp-140][si]
	mov	WORD PTR [bp-146],ax	;reloc_ptr
	mov	WORD PTR [bp-144],ss
;                    target_ptr = MK_FP(pspaddr + 0x10 + reloc_ptr->segment,
	les	bx,DWORD PTR [bp-146]	;reloc_ptr
	mov	ax,WORD PTR es:[bx+2]
RDOVLY_TEXT      ENDS
CONST      SEGMENT
$T20002	DW SEG _pspaddr 
CONST      ENDS
RDOVLY_TEXT      SEGMENT
	ASSUME	CS: RDOVLY_TEXT
	mov	es,$T20002
	add	ax,es:_pspaddr
	add	ax,16
	sub	dx,dx
	mov	dx,ax
	sub	ax,ax
	les	bx,DWORD PTR [bp-146]	;reloc_ptr
	or	ax,WORD PTR es:[bx]
	mov	WORD PTR [bp-154],ax	;target_ptr
	mov	WORD PTR [bp-152],dx
;                                       reloc_ptr->offset);
;                    *target_ptr += pspaddr + 0x10;
	mov	es,$T20002
	mov	ax,es:_pspaddr
	add	ax,16
	les	bx,DWORD PTR [bp-154]	;target_ptr
	add	WORD PTR es:[bx],ax
;                }
	jmp	$FC167
$FB168:
;            }
	jmp	$FC164
$FB165:
;            myclose(executable_handle);
	push	WORD PTR [bp-156]	;executable_handle
	call	FAR PTR _myclose
	add	sp,2
;            return 0;
	mov	ax,0
	jmp	$EX139
;        } else {
	jmp	$I169
$I162:
;	        nextpos = startpos + sectionsize;
	mov	ax,WORD PTR [bp-8]	;startpos
	mov	dx,WORD PTR [bp-6]
	add	ax,WORD PTR [bp-4]	;sectionsize
	adc	dx,WORD PTR [bp-2]
	mov	WORD PTR [bp-12],ax	;nextpos
	mov	WORD PTR [bp-10],dx
;            /* round up to 512-byte bound */
;            nextpos = (nextpos + 511L) & ~511L;
	mov	ax,WORD PTR [bp-12]	;nextpos
	mov	dx,WORD PTR [bp-10]
	add	ax,511
	adc	dx,0
	and	ax,-512
	and	dx,-1
	mov	WORD PTR [bp-12],ax	;nextpos
	mov	WORD PTR [bp-10],dx
;	        if (nextpos >= filesize) {
	mov	ax,WORD PTR [bp-150]	;filesize
	mov	dx,WORD PTR [bp-148]
	cmp	WORD PTR [bp-10],dx
	jge	$JCC646
	jmp	$I170
$JCC646:
	jle	$JCC651
	jmp	$L20003
$JCC651:
	cmp	WORD PTR [bp-12],ax	;nextpos
	jae	$JCC659
	jmp	$I170
$JCC659:
$L20003:
;                myclose(executable_handle);
	push	WORD PTR [bp-156]	;executable_handle
	call	FAR PTR _myclose
	add	sp,2
;	            return 1;
	mov	ax,1
	jmp	$EX139
;            }
;	        mylseek(executable_handle,nextpos,0);
$I170:
	mov	ax,0
	push	ax
	push	WORD PTR [bp-10]
	push	WORD PTR [bp-12]	;nextpos
	push	WORD PTR [bp-156]	;executable_handle
	call	FAR PTR _mylseek
	add	sp,8
;        }
$I169:
;    }
	jmp	$FC153
$FB154:
;    myclose(executable_handle);
	push	WORD PTR [bp-156]	;executable_handle
	call	FAR PTR _myclose
	add	sp,2
;    return 1;
	mov	ax,1
	jmp	$EX139
;}
$EX139:
	pop	si
	pop	di
	mov	sp,bp
	pop	bp
	ret	

_read_overlay_section	ENDP
	nop	
RDOVLY_TEXT	ENDS
END
