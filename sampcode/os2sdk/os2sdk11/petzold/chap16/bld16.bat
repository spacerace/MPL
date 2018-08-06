rem Order is important because SQBTNLIB must be built before BUTTONS3
rem
rem Build the DLLs
rem
make BITLIB
make HDRLIB
make SQBTNLIB
rem
rem Build the apps
rem 
make SHOWBIT
make HDRTEST
make BUTTONS3
rem
rem Get rid of unnecessary files
rem
del *.lib
del *.obj
del *.res
