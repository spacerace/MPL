
# Figure 7
# ========

#
# Program: Gdemo
#

.c.obj:
	qcl -c  -W2 -Ze -AM $*.c

gdemo.obj : gdemo.c

message.obj : message.c

Gdemo.exe : gdemo.obj message.obj 
	del Gdemo.lnk
	echo gdemo.obj+ >>Gdemo.lnk
	echo message.obj  >>Gdemo.lnk
	echo Gdemo.exe >>Gdemo.lnk
	echo Gdemo.map >>Gdemo.lnk
	link @Gdemo.lnk $(LDFLAGS);
