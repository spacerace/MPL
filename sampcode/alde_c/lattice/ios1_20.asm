title	Level 1 I/O Functions for Lattice 'c' Version 1.04
page	64,132
name	IOS1_20 ;DOS function calls 3DH, 3EH, 3FH, 40H, AND 42H.
comment |
   These functions take advantage of DOS 2.0 and will directly
replace the original level 1 I/O except that 'text mode' is
not supported.	DOS error numbers are saved at ERRNO and may be
checked by including: "extern short errno;" in your source files.
Note that the pmode parameter in creat is forced to zero, due to
a problem with Lattice's freopen which passes a 1A4H to creat.

	     Ted Reuss	   c/o South Texas Software, Inc.
	Home: 713/961-3926	4544 Post Oak Place, Suite 176
	Offi: 713/877-8205	Houston, Tx 77027
	|
	public	CREAT, OPEN, CLOSE, READ, WRITE, LSEEK
dgroup	group	data
data	segment word public 'data'
	assume	ds:dgroup

	public	ERRNO
ERRNO	dw	0	; DOS error number (DOS Manual page D-14)
data	ends

pgroup	group	prog
prog	segment byte public 'prog'
	assume	cs:pgroup

	subttl	CREAT -- create a new file
	page
;
; NAME
;	creat -- create a new file
;
; SYNOPSIS
;
;	file = creat(name, pmode);
;	int file;	file number or error code
;	char *name;	file name (valid drive\path\filespec)
;	int pmode;	access privilege mode bits, DOS attribute
;			NOTE: forced to zero in current version.
;
; DESCRIPTION
;
;	Creates a new file with the specified name and prepares is
;	for access via the level 1 I/O functions.  The file name
;	may consist of a valid drive and path name.  All I/O is
;	done via DOS calls 3fh (READ) and 40h (WRITE).	If the file
;	already exists, it's contents are discarded.  The current file
;	position and the end of file are both zero. (indicating an
;	empty file) if the function is successful. ERRNO is set to
;	the error number returned by DOS in the event of an error.
;
; RETURNS
;	file = file number to access file, if successful
;	     = -1 if error  (ERRNO get DOS error number)
;

	public	CREAT
CREAT	proc	near
	push	bp
	mov	bp,sp
	mov	dx,[bp+4]	;get ptr to drive\path\filespec
	xor	cx,cx
	mov	ah,3CH		;DOS create function
	int	21H
	jnc	cre10
	mov	dgroup:errno,ax
	mov	ax,-1
cre10:	mov	sp,bp
	pop	bp
	ret
CREAT	endp

	subttl	OPEN -- open a file
	page
;
; NAME
;	open -- open a file
;
; SYNOPSIS
;
;	file = open(name, rwmode);
;	int file;	file number or error code
;	char *name;	file name (valid drive\path\filespec)
;	int rwmode;	read/write mode, where 0=read, 1=write,
;			2=read/write
;
; DESCRIPTION
;
;	Opens a file for access using the level 1 I/O functions.
;	The file name may contain a valid drive and path name.	All
;	I/O is done via DOS functions 3fh (READ) and 40h (WRITE).
;	The mode word determines the type of I/O which will be
;	performed on the file.	The low order bits specify whether
;	read or write operations (or both) are to be allowed.
;	In the event of an error the error code passed by DOS is
;	saved at ERRNO.
;
; RETURNS
;
;	file = file number to access file, if successful
;	     = -1 if error  (ERRNO get DOS error number)
;

	public	OPEN
OPEN	proc	near
	push	bp
	mov	bp,sp
	mov	dx,[bp+4]	;get ptr to drive\path\filespec
	mov	ax,[bp+6]	;get mode
	mov	ah,3DH		;DOS open function
	int	21H
	jnc	opn10
	mov	dgroup:errno,ax
	mov	ax,-1
opn10:	mov	sp,bp
	pop	bp
	ret
OPEN	endp

	subttl	CLOSE -- close a file
	page
;
; NAME
;
;	close -- close a file
;
; SYNOPSIS
;
;	status = close(file);
;	int status;	status code: 0 if successful
;	int file;	file number for file
;
; DESCRIPTION
;
;	Close a file and frees the file number for use in accessing
;	another file.  Any buffers allocated when the file was
;	opened are released.
;
; RETURNS
;
;	status = 0 if successful
;	       = -1 if error  (ERRNO get DOS error number)
;

	public	CLOSE
CLOSE	proc	near
	push	bp
	mov	bp,sp
	mov	bx,[bp+4]	;get file handle
	mov	ah,3EH		;DOS close function
	int	21H
	jc     clo10
	xor	ax,ax
	jmp	short clo20
clo10:	mov	dgroup:errno,ax
	mov	ax,-1
clo20:	mov	sp,bp
	pop	bp
	ret
CLOSE	endp

	subttl	READ -- read data from file
	page
;
; NAME
;
;	read -- read data from file
;
; SYNOPSIS
;
;	status = read(file, buffer, length);
;	int status;	status code or actual length
;	int file;	file number for file
;	char *buffer;	input buffer
;	int length;	number of bytes requested
;
; DESCRIPTION
;
;	Reads next set of bytes from a file.  The return count
;	is always equal to the number of bytes placed in the buffer
;	and will never exceed the "length" parameter, except in the
;	case of an error, where -1 is returned.  The file position
;	is advanced accordingly.
;
; RETURNS
;
;	status = 0 if end of file
;	       = -1 if error  (ERRNO get DOS error number)
;	       = else number of bytes actually read
;

	public	READ
READ	proc	near
	push	bp
	mov	bp,sp
	mov	bx,[bp+4]	;get file handle
	mov	dx,[bp+6]	;get buffer address
	mov	cx,[bp+8]	;get byte count
	mov	ah,3FH		;DOS read function
	int	21H
	jnc	red10
	mov	dgroup:errno,ax
	mov	ax,-1
red10:	mov	sp,bp
	pop	bp
	ret
READ	endp

	subttl	WRITE -- write data to file
	page
;
; NAME
;
;	write -- write data to file
;
; SYNOPSIS
;
;	status = write(file, buffer, length);
;	int status;	status code or actual length
;	int file;	file number for file
;	char *buffer;	output buffer
;	int length;	number of bytes in buffer
;
; DESCRIPTION
;
;	Writes next set of bytes to a file.  The return count is
;	equal to the number of bytes written, unless an error
;	occurred.  The file position is advanced accordingly.
;
; RETURNS
;
;	status = -1 if error  (ERRNO get DOS error number)
;	       = else number of bytes actually written
;

	public	WRITE
WRITE	proc	near
	push	bp
	mov	bp,sp
	mov	bx,[bp+4]	;get file handle
	mov	dx,[bp+6]	;get buffer address
	mov	cx,[bp+8]	;get byte count
	mov	ah,40H		;DOS write function
	int	21H
	jnc	wrt10
	mov	dgroup:errno,ax
	mov	ax,-1
wrt10:	mov	sp,bp
	pop	bp
	ret
WRITE	endp

	subttl	LSEEK -- seek to specified file position
	page
;
; NAME
;
;	lseek -- seek to specified file position
;
; SYNOPSIS
;
;	pos = lseek(file, offset, mode);
;	long pos;	returned file position or error code
;	int file;	file number for file
;	long offset;	desired position
;	int mode;	offset mode relative to:
;			0 = beginning of file	    BOFM
;			1 = current file position   CURM
;			2 = end of file 	    EOFM
;
; DESCRIPTION
;
;	Changes the current file position to a new position in the
;	file.  The offset is specified as a long int and is added to
;	the current position (mode 1) or to the logical end of file
;	(mode 2).  Use DOS function 42h (LSEEK).
;
; RETURNS
;
;	pos = -1L if error occurred (ERRNO get DOS error number)
;	= new file position if successful
;

	public	LSEEK
LSEEK	proc	near
	push	bp
	mov	bp,sp
	mov	bx,[bp+4]	;get file handle
	mov	dx,[bp+6]	;get low word of pos
	mov	cx,[bp+8]	;get high word of pos
	mov	ax,[bp+10]	;get seek mode
	mov	ah,42H		;DOS lseek function
	int	21H
	jnc	lsk10
	mov	dgroup:errno,ax
	mov	ax,-1
	mov	dx,ax
lsk10:	mov	bx,ax
	mov	ax,dx
	mov	sp,bp
	pop	bp
	ret
LSEEK	endp

prog	ends
	end
