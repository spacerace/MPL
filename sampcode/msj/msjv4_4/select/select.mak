cp=cl -c -Ownlt -Alnw -Gsw -G2 -Zpe -Zl

libentry.obj: libentry.asm
  masm libentry ;

libmain.obj: libmain.c
  $(cp) -NT _INIT libmain.c

select.obj: select.c
  $(cp) -NT _SELECT select.c

select.exe: libentry.obj libmain.obj select.obj select.def
  link4 @select.lnk
  copy select.exe \surf\bin


