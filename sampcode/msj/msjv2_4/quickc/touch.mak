
# Figure 9
# ========


#
# Program: Touch
#

.c.obj:
	qcl -c  -Iusage.c -W0 -Ze -Zid -AM $*.c

usage.obj : usage.c

touch.obj : touch.c

Touch.exe : usage.obj touch.obj 
	del Touch.lnk
	echo usage.obj+ >>Touch.lnk
	echo touch.obj  >>Touch.lnk
	echo Touch.exe >>Touch.lnk
	echo Touch.map >>Touch.lnk
	link @Touch.lnk;


# Link Information - Touch.lnk
# ============================
# touch.obj+
# usage.obj
# Touch.exe
# Touch.map
