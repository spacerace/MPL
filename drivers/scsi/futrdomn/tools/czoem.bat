@echo off
cls
echo !This batch file complies the test routine for the direct oem
echo !	interface.
echo !
echo !   
echo !If you DO NOT want to proceed, hit CTRL BREAK.
pause

IF  X%1 == XLARGE  goto lrg
IF  X%1 == Xlarge  goto lrg
IF  X%1 == XLarge  goto lrg
IF  X%1 == XMEDIUM goto mdm
IF  X%1 == Xmedium goto mdm
IF  X%1 == XMedium goto mdm

echo ! Compiling for SMALL model
cl  /AS /Zp /c oemprint.c
cl  /AS /Zp oemtst.c  oemprint.obj oemscsi.obj
goto cont
:mdm
echo ! Compiling for MEDIUM model
cl  /AM /Zp /c oemprint.c
cl  /AM /Zp oemtst.c  oemprint.obj oemscsim.obj
goto cont
:lrg
echo ! Compiling for LARGE model
cl  /AL /Zp /c oemprint.c
cl  /AL /Zp oemtst.c  oemprint.obj oemscsil.obj
:cont
echo !Compile is complete.  Type OEMTST to execute.
goto done
:err
echo ! Sorry, you should specify the model : SMALL or MEDIUM or LARGE
echo ! Enter one of the following next time:
echo !
echo ! CZOEM  SMALL
echo ! or
echo ! CZOEM  MEDIUM
echo ! or
echo ! CZOEM  LARGE
:done

