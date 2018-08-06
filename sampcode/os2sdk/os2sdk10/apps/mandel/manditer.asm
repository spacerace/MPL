	page	,132
	title	manditer - compute next iteration of mandelbrot set
	%out	manditer

;	Conditional assembly flags:
;
;	TARGET_287	Defined if the target machine will be a 286
;			with a 287.  This allows the 287 specific
;			instruction FSTSW AX to be used.
;
;	DO_SCAN 	Defined if an entire scan is to be processed.
;			If so, extra parameters are supplied consisting
;			of the increment for the real portion of the
;			seed, a word buffer where the results will be
;			stored, and the count of points to compute.

TARGET_287	equ	0
DO_SCAN 	equ	0

ifdef	TARGET_287
	.286c
	.287
else
	.8087
endif


memS	equ	1			;Define small model program
?PLM	=	0			;Define 'C' calling convention
?WIN	=	0			;Define non-windows prologue

	.xlist
	include cmacros.inc
	.list


sBegin	code
assumes cs,code
assumes ds,nothing

four	dq	4.0			;Constant 4.0


;	The Mandelbrot set is defined to be the set of all complex
;	numbers C for which the size of Z^2 + C is finite after an
;	indefinate number of iterations (Z is initially 0).
;
;	The square of a complex number C = C_Real + C_imag is
;
;		(C_Real^2) - (2*C_Real*C_Imag)i - (C_Imag^2)
;
;
;	The size of a complex number is just it's distance from
;	the complex number 0.  The distance from complex 0 is
;	the hypotenuse of the right triangle whose sides are the
;	real and imaginary parts of the complex number.  Thus the
;	size is SQRT(Z_real^2 + Z_imag).  If the size ever becomes
;	>= 2, the number will grow to infinity.



cProc	manditer,<PUBLIC>

	parmq	creal			;Real part of mandelbrot seed
	parmq	cimag			;Imaginary part of mandelbrot seed
	parmw	max_iteration		;Maximum loop counter

	ifdef	DO_SCAN 		;If to do an entire scan line
	parmw	point_count		;Number of points to calculate
	parmw	results_buffer		;Near pointer to where results go
	parmq	creal_step		;Increment to next point
	endif

	ifndef	TARGET_287		;If not explicitly for a 287
	localw	fstatus 		;x87 status word
	endif

cBegin
	ifdef	DO_SCAN 		;If to do an entire scan line
	push	si			;'C' requires SI to be saved
	mov	si,results_buffer	;DS:SI --> destination buffer
	mov	dx,point_count		;# of points to calculate
	endif

	finit				;Initialize co-porcessor

	mov	bx,max_iteration	;Max iteration counter

	fld	four			;Load constant 4.0 for comparison
;      |-------------------------------|
;      | 4			       | st(0)
;      |-------------------------------|


	fld	creal			;Load real part of seed
;      |-------------------------------|
;      | C real 		       | st(1)
;      |-------------------------------|
;      | 4			       | st(2)
;      |-------------------------------|


	fld	cimag			;Load imaginary part of seed
;      |-------------------------------|
;      | C imag 		       | st(0)
;      |-------------------------------|
;      | C real 		       | st(1)
;      |-------------------------------|
;      | 4			       | st(2)
;      |-------------------------------|


	ifdef	DO_SCAN 		;If to do an entire scan
	fld	creal_step		;  load creal increment
	else				;else
	fdecstp 			;  Reserve location, tag as invalid
	endif
;      |-------------------------------|
;      | Reserved or C real increment  | st(0)
;      |-------------------------------|
;      | C imag 		       | st(1)
;      |-------------------------------|
;      | C real 		       | st(2)
;      |-------------------------------|
;      | 4			       | st(3)
;      |-------------------------------|


scan_loop:

;	Because this program optimizes the use of C real ^ 2
;	and C imag ^ 2, the size test will terminate one iteration
;	late.  The original Mandelzoom was:
;
;	    Z = 0
;	    Count = 0;
;	    repeat
;		Z = Z^2 + C
;		count = count + 1
;		size = size of Z
;	    until (count > 1000 or size >= 4)
;
;	This program uses the intermediate calculations used for
;	the squaring for the size test.  However, the result of
;	the first complex square operation will simply be the
;	constant C (0^2+C).  By setting the initial value of Z
;	to C instead of 0, this latency will be corrected.



	fld	st(1)			;Initialize imaginary part of sum
;      |-------------------------------|
;      | Z imag = C imag	       | st(0)
;      |-------------------------------|
;      | Reserved or C real increment  | st(1)
;      |-------------------------------|
;      | C imag 		       | st(2)
;      |-------------------------------|
;      | C real 		       | st(3)
;      |-------------------------------|
;      | 4			       | st(4)
;      |-------------------------------|


	fld	st(3)			;Initialize real part of sum
;      |-------------------------------|
;      | Z real = C real	       | st(0)
;      |-------------------------------|
;      | Z imag = C image	       | st(1)
;      |-------------------------------|
;      | Reserved or C real increment  | st(2)
;      |-------------------------------|
;      | C imag 		       | st(3)
;      |-------------------------------|
;      | C real 		       | st(4)
;      |-------------------------------|
;      | 4			       | st(5)
;      |-------------------------------|


	mov	cx,bx			;max iteration counter

iterate_loop:


	fld	st(0)
;      |-------------------------------|
;      | Z real 		       | st(0)
;      |-------------------------------|
;      | Z real 		       | st(1)
;      |-------------------------------|
;      | Z imag 		       | st(2)
;      |-------------------------------|
;      | Reserved or C real increment  | st(3)
;      |-------------------------------|
;      | C imag 		       | st(4)
;      |-------------------------------|
;      | C real 		       | st(5)
;      |-------------------------------|
;      | 4			       | st(6)
;      |-------------------------------|


	fmul	st(1),st
;      |-------------------------------|
;      | Z real 		       | st(0)
;      |-------------------------------|
;      | (Z real)^2		       | st(1)
;      |-------------------------------|
;      | Z imag 		       | st(2)
;      |-------------------------------|
;      | Reserved or C real increment  | st(3)
;      |-------------------------------|
;      | C imag 		       | st(4)
;      |-------------------------------|
;      | C real 		       | st(5)
;      |-------------------------------|
;      | 4			       | st(6)
;      |-------------------------------|


	fmul	st,st(2)
;      |-------------------------------|
;      | Z real * Z imag	       | st(0)
;      |-------------------------------|
;      | (Z real)^2		       | st(1)
;      |-------------------------------|
;      | Z imag 		       | st(2)
;      |-------------------------------|
;      | Reserved or C real increment  | st(3)
;      |-------------------------------|
;      | C imag 		       | st(4)
;      |-------------------------------|
;      | C real 		       | st(5)
;      |-------------------------------|
;      | 4			       | st(6)
;      |-------------------------------|


	fadd	st,st(0)
;      |-------------------------------|
;      | Z real * Z imag * 2	       | st(0)
;      |-------------------------------|
;      | Z real ^ 2		       | st(1)
;      |-------------------------------|
;      | Z imag 		       | st(2)
;      |-------------------------------|
;      | Reserved or C real increment  | st(3)
;      |-------------------------------|
;      | C imag 		       | st(4)
;      |-------------------------------|
;      | C real 		       | st(5)
;      |-------------------------------|
;      | 4			       | st(6)
;      |-------------------------------|


	fadd	st,st(4)
;      |-------------------------------|
;      | Z real * Z imag * 2 + C imag  | st(0)
;      |-------------------------------|
;      | Z real ^ 2		       | st(1)
;      |-------------------------------|
;      | Z imag 		       | st(2)
;      |-------------------------------|
;      | Reserved or C real increment  | st(3)
;      |-------------------------------|
;      | C imag 		       | st(4)
;      |-------------------------------|
;      | C real 		       | st(5)
;      |-------------------------------|
;      | 4			       | st(6)
;      |-------------------------------|


	fxch	st(2)
;      |-------------------------------|
;      | Z imag 		       | st(0)
;      |-------------------------------|
;      | Z real ^ 2		       | st(1)
;      |-------------------------------|
;      | Z real * Z imag * 2 + C imag  | st(2)
;      |-------------------------------|
;      | Reserved or C real increment  | st(3)
;      |-------------------------------|
;      | C imag 		       | st(4)
;      |-------------------------------|
;      | C real 		       | st(5)
;      |-------------------------------|
;      | 4			       | st(6)
;      |-------------------------------|


	fmul	st,st(0)
;      |-------------------------------|
;      | Z imag ^ 2		       | st(0)
;      |-------------------------------|
;      | Z real ^ 2		       | st(1)
;      |-------------------------------|
;      | Z real * Z imag * 2 + C imag  | st(2)
;      |-------------------------------|
;      | Reserved or C real increment  | st(3)
;      |-------------------------------|
;      | C imag 		       | st(4)
;      |-------------------------------|
;      | C real 		       | st(5)
;      |-------------------------------|
;      | 4			       | st(6)
;      |-------------------------------|


	fld	st(1)
;      |-------------------------------|
;      | Z real ^ 2		       | st(0)
;      |-------------------------------|
;      | Z imag ^ 2		       | st(1)
;      |-------------------------------|
;      | Z real ^ 2		       | st(2)
;      |-------------------------------|
;      | Z real * Z imag * 2 + C imag  | st(3)
;      |-------------------------------|
;      | Reserved or C real increment  | st(4)
;      |-------------------------------|
;      | C imag 		       | st(5)
;      |-------------------------------|
;      | C real 		       | st(6)
;      |-------------------------------|
;      | 4			       | st(7)
;      |-------------------------------|


	fadd	st,st(1)
;      |-------------------------------|
;      | Z real ^ 2 + Z imag ^ 2       | st(0)
;      |-------------------------------|
;      | Z imag ^ 2		       | st(1)
;      |-------------------------------|
;      | Z real ^ 2		       | st(2)
;      |-------------------------------|
;      | Z real * Z imag * 2 + C imag  | st(3)
;      |-------------------------------|
;      | Reserved or C real increment  | st(4)
;      |-------------------------------|
;      | C imag 		       | st(5)
;      |-------------------------------|
;      | C real 		       | st(6)
;      |-------------------------------|
;      | 4			       | st(7)
;      |-------------------------------|


	fcomp	st(7)
;      |-------------------------------|
;      | Z imag ^ 2		       | st(0)
;      |-------------------------------|
;      | Z real ^ 2		       | st(1)
;      |-------------------------------|
;      | Z real * Z imag * 2 + C imag  | st(2)
;      |-------------------------------|
;      | Reserved or C real increment  | st(3)
;      |-------------------------------|
;      | C imag 		       | st(4)
;      |-------------------------------|
;      | C real 		       | st(5)
;      |-------------------------------|
;      | 4			       | st(6)
;      |-------------------------------|


	ifdef	TARGET_287		;If 287, then store status word
	fwait				;  straight to ax.  Have to wait!
	fstsw	ax			;
	else				;else
	fstsw	fstatus 		;  will have to store to memory
	endif
;      |-------------------------------|
;      | Z imag ^ 2		       | st(0)
;      |-------------------------------|
;      | Z real ^ 2		       | st(1)
;      |-------------------------------|
;      | Z real * Z imag * 2 + C imag  | st(2)
;      |-------------------------------|
;      | Reserved or C real increment  | st(3)
;      |-------------------------------|
;      | C imag 		       | st(4)
;      |-------------------------------|
;      | C real 		       | st(5)
;      |-------------------------------|
;      | 4			       | st(6)
;      |-------------------------------|


	fsubp	st(1),st
;      |-------------------------------|
;      | Z real ^ 2 - Z imag ^ 2       | st(0)
;      |-------------------------------|
;      | Z real * Z imag * 2 + C imag  | st(1)
;      |-------------------------------|
;      | Reserved or C real increment  | st(2)
;      |-------------------------------|
;      | C imag 		       | st(3)
;      |-------------------------------|
;      | C real 		       | st(4)
;      |-------------------------------|
;      | 4			       | st(5)
;      |-------------------------------|



;	Do a little work while the FSUBP executes

	ifndef	TARGET_287		;If not a 287
	mov	ah,byte ptr (fstatus+1) ;  load status word from memory
	endif

	and	ah,045h 		;If size>= 4, outside of mandelbrot
	xor	ah,1			;  set.
	jnz	done_iterating		;Outside of set


	fadd	st,st(4)
;      |-------------------------------|
;      | Z real^2 - Z imag^2 + C real  | st(0)
;      |-------------------------------|
;      | Z real * Z imag * 2 + C imag  | st(1)
;      |-------------------------------|
;      | Reserved or C real increment  | st(2)
;      |-------------------------------|
;      | C imag 		       | st(3)
;      |-------------------------------|
;      | C real 		       | st(4)
;      |-------------------------------|
;      | 4			       | st(5)
;      |-------------------------------|


	loop	iterate_loop		;If loop expires, in mandelbrot set

done_iterating:
	mov	ax,bx			;Max iteration count
	sub	ax,cx			;Compute number of iterations

	ifdef	DO_SCAN 		;If to do an entire scan
	mov	word ptr [si],ax	;Save result in result_buffer
	add	si,2			;--> next slot of buffer
	dec	dx			;Any more points?
	jz	done_scan		;No, all done

	fstp	st(0)			;Remove sum from stack
	fstp	st(0)
;      |-------------------------------|
;      | Reserved or C real increment  | st(0)
;      |-------------------------------|
;      | C imag 		       | st(1)
;      |-------------------------------|
;      | C real 		       | st(2)
;      |-------------------------------|
;      | 4			       | st(3)
;      |-------------------------------|


	fadd	st(2),st		;Update c real
;      |-------------------------------|
;      | Reserved or C real increment  | st(0)
;      |-------------------------------|
;      | C imag 		       | st(1)
;      |-------------------------------|
;      | C real + C real increment     | st(2)
;      |-------------------------------|
;      | 4			       | st(3)
;      |-------------------------------|

	jmp	scan_loop

done_scan:
	pop	si			;Return last result
	endif


cEnd

sEnd	code
end
