echo off
echo Computer Innovations (Small Model)
cc1 %1 %2 %3 %4 -cm -dC86
if errorlevel 1 goto done
cc2 %1
if errorlevel 1 goto done
cc3 %1
if errorlevel 1 goto done
cc4 %1
if errorlevel 1 goto done
:done
