echo off
cls
rem  One-shot installation, minimal error checking  --mzu 07/29/88
echo PC-Project   --Copyright 1988, The Whitewater Group, Inc.
echo A Windows 2.x demonstration program written in Actor
echo PC-Project may be freely distributed
echo �
if NOT %1x==x goto c2
:c1
echo Parameters missing or incorrect.  The correct syntax is:
echo �
echo      INSTALL source: target: target:target-pathname
echo �
echo e.g. INSTALL A: C: C:\ACTOR
echo �
echo where
echo  * source: is the floppy drive where the PC-Project system will be
echo    loaded from (such as A: or B:)
echo  * target: is the drive where Actor will be installed (such as C:)
echo  * target:target-pathname is the directory where PC-Project will be
echo    installed (such as C:\ACTOR)
echo �
goto serror
:c2
if %1==a: goto ok1
if %1==A: goto ok1
if %1==b: goto ok1
if %1==B: goto ok1
if %1==c: goto ok1
if %1==C: goto ok1
if %1==d: goto ok1
if %1==D: goto ok1
if %1==e: goto ok1
if %1==E: goto ok1
echo The source: is not A:,B:,C:,D: or E:.
goto c1
:ok1
if NOT %2x==x goto c2a
echo The target: was not specified.
goto c1
:c2a
if %1==a: goto ok2
if %1==A: goto ok2
if %1==b: goto ok2
if %1==B: goto ok2
if %1==c: goto ok2
if %1==C: goto ok2
if %1==d: goto ok2
if %1==D: goto ok2
if %1==e: goto ok2
if %1==E: goto ok2
echo The target: is not A:,B:,C:,D: or E:.
goto c1
:ok2
if NOT %3x==x goto c3
echo The target:target-pathname was not specified.
goto c1
:c3
:c4
rem ...... put messages here ......
echo �
echo The installation of PC-Project will overwrite all files in %3
echo and its subdirectories which have similar names as those on
echo the PC-Project installation disk.  If you do not want to have these
echo files erased, you can exit this installation program at this time
echo with a CONTROL-C (^C) and then back up the files which you want
echo to preserve.  You can then rerun this installation program.
echo �
echo  Note: Ignore any "unable to create directory" error messages
echo �
echo Press CONTROL-C (^C) to ABORT, any other key to continue...
pause >NUL
rem ......... end messages ........
:c5
echo �
echo Creating the PC-Project directory -- %3.
md %3 > nul
echo Changing directory to %3....
%2
cd %3
echo �
echo Copying ZOO.EXE from %1
copy %1zoo.exe > nul
echo �
REM 04/21/88 mzu   uses ZOO xO/ to copy to directories
echo Expanding PC-Project...
zoo xO/ %1project.zoo project.exe
zoo xO/ %1project.zoo project.ima
zoo xO/ %1project.zoo *.prj
zoo xO/ %1project.zoo *.txt
copy %1*.TXT > nul
echo �
echo Please read the files READPROJ.TXT and PMANUAL.TXT for instructions.
echo �
echo Press [Return] to install source code, Ctrl-C to cancel
pause > nul
echo �
echo Creating subdirectories
md CLASSES > nul
md ACT > nul
md RES > nul
echo �
echo Expanding source code...
zoo xO/ %1project.zoo *.cls
zoo xO/ %1project.zoo *.act
zoo xO/ %1project.zoo *.lod
zoo xO/ %1project.zoo *.h
zoo xO/ %1project.zoo *.rc
zoo xO/ %1project.zoo *.ico
:finish
cls
echo �
echo You have now installed PC-Project.  You will be left in the
echo directory from which you can run it.  At the DOS prompt type
echo �
echo       WIN PROJECT (CR)                where (CR) is the Return key
echo �
echo in the current directory -- %3.
echo �
echo Please read the files READPROJ.TXT and PMANUAL.TXT for instructions.
goto :done
:serror
echo �
echo Installation terminated because of errors.
:done
