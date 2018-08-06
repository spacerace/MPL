if not "%1" == "" goto FAST

nmake -i CFLAGS="-DCHECKASSERTS -Zi" LFLAGS=/CO 2>&1 | tee log
goto EXIT

:FAST

nmake -i 2>&1 | tee log

:EXIT
qgrep -y err log
qgrep -y war log
