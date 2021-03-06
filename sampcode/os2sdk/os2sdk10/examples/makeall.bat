rem
rem .BAT file to build all of the demonstration programs.  Note: since
rem it is not possible to specify the library path for the C compiler
rem linker we need to make it an environmental variable.
rem
rem The variables are lost when the .BAT file exits.
rem

set INCLUDE=..\..\..\include
set LIB=..\..\..\lib

cd ALLOC
  make ALLOC
cd ..
cd ARGUMENT
  make ARGUMENT
cd ..
cd ASMEXMPL
  make ASMEXMPL
cd ..
cd ASYNCIO
  make ASYNCIO
cd ..
cd BEEPC
  make BEEPC
cd ..
cd CRITSEC
  make CRITSEC
cd ..
cd CONFIG
  make CONFIG
cd ..
cd COUNTRY
  make COUNTRY
cd ..
cd CSALIAS
  make CSALIAS
cd ..
cd CWAIT
  make CWAIT
cd ..
cd DATETIME
  make DATETIME
cd ..
cd DOSEXIT
  make DOSEXIT
cd ..
cd DYNLINK
  make DYNLINK
cd ..
cd EXITLIST
  make EXITLIST
cd ..
cd FSINFO
  make FSINFO
cd ..
cd GETENV
  make GETENV
cd ..
cd HELLO
  make HELLO
cd ..
cd HUGE
  make HUGE
cd ..
cd INFOSEG
  make INFOSEG
cd ..
cd IOPL
  make IOPL
cd ..
cd KEYS
  make KEYS
cd ..
cd KILL
  make KILL
cd ..
cd MACHMODE
  make MACHMODE
cd ..
cd MONITORS
  make MONITORS
cd ..
cd MOVE
  make MOVE
cd ..
cd PIPES
  make PIPES
cd ..
cd QHTYPE
  make QHTYPE
cd ..
cd QUEUES
  make QUEUES
cd ..
cd REALLOC
  make REALLOC
cd ..
cd SESSION
  make SESSION
cd ..
cd SETMAXFH
  make SETMAXFH
cd ..
cd SETVEC
  make SETVEC
cd ..
cd SHARE
  make SHARE
cd ..
cd SIGNAL
  make SIGNAL
cd ..
cd SLEEP
  make SLEEP
cd ..
cd SUBALLOC
  make SUBALLOC
cd ..
cd SUSPEND
  make SUSPEND
cd ..
cd THREADS
  make THREADS
cd ..
cd TIMER
  make TIMER
cd ..
cd VERSION
  make VERSION
cd ..
