rem Make OPENDLG first because other samples need it
rem
call makesamp opendlg
call makesamp accel
call makesamp bigben
call makesamp bio
cd browse
call makebrow
cd ..
call makesamp calc
cd calc
call ..\makesamp dcalc
cd ..
call makesamp cascade
call makesamp chaser
call makesamp clipview
call makesamp clock
call makesamp comtalk
call makesamp cpgrep
call makesamp ddeml
cd ddemgr
call ..\makesamp ddespy
call ..\makesamp monitor
call ..\makesamp msngr
call ..\makesamp demo
cd ..
call makesamp dlgsamp
call makesamp edpline
call makesamp expand
call makesamp fatpel
call makesamp fdir
call makesamp hanoi
call makesamp iniedit
call makesamp jigsaw
call makesamp linefrac
call makesamp mdi
call makesamp msgbox
call makesamp nead
cd opendlg
call ..\makesamp hello
cd ..
call makesamp pipes
call makesamp pmcap
call makesamp share
call makesamp snap
call makesamp sort
call makesamp spy
call makesamp suspend
call makesamp ted
call makesamp terminal
call makesamp vectfont
call makesamp wmchar
call makesamp stock
