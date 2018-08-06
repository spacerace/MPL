	INTERFACE TO INTEGER[C] FUNCTION getmod[C]
	END
C
C
	INTERFACE TO SUBROUTINE init[C](num)
	INTEGER[C] num
	END
C
C
	INTERFACE TO SUBROUTINE setbck[C](num)
	INTEGER[C] num
	END
C
C
	INTERFACE TO SUBROUTINE palett[C](num)
	INTEGER[C] num
	END
C
C
	INTERFACE TO SUBROUTINE circle[C](x, y, rad, col)
	INTEGER[C] x, y, rad, col
	END
C
C
C	Change "back" between 1 and 15 and "pal" between 0 and 1 to
C	get different results.
C
	PROGRAM graph
	INTEGER[C] back/0/,pal/1/, imode, mode/4/, getmod
	INTEGER xmax/320/, ymax/200/, radmax/18/, xcenter/160/
	INTEGER y, xoff, radius, color, bumps/2/
	INTEGER xoffs(4)/0, 46, 92, 140/
	REAL pi
	PARAMETER (pi = 3.141569265)
	imode = getmod()
	CALL init(mode)
	CALL setbck(back)
	CALL palett(pal)
	DO 30 i = 1, 3
		DO 20 y = 1, ymax
			r      = (REAL(y)/ymax)*pi*bumps
			x      = SIN(r)
			radius = radmax * ABS(x)
			DO 10 j = 1, 4
					xoff  = xoffs(j) * x
					color = MOD(j+i-1, 3)+1
					CALL mirror(xcenter, xoff, y, radius, color)
10			CONTINUE
20		CONTINUE
30	CONTINUE
	DO 40 j = 1,300000
		CALL timer
40	CONTINUE
	CALL init(imode)
	END
C
C
	SUBROUTINE mirror(xcenter, xoff, y, radius, color)
	IMPLICIT INTEGER (a-z) 
	CALL circle(xcenter+xoff, y, radius, color)
	CALL circle(xcenter-xoff, y, radius, color)
	END
C
C
	SUBROUTINE timer
	END
