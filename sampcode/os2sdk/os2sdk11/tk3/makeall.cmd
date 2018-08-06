rem Make OPENDLG first because other samples need it
rem
call makesamp opendlg
call makesamp bio
cd browse
call makebrow
cd ..
call makesamp calc
cd calc
call ..\makesamp dcalc
cd ..
call makesamp clock
call makesamp comtalk
call makesamp dlgsamp
call makesamp edpline
call makesamp fatpel
call makesamp iniedit
call makesamp jigsaw
call makesamp linefrac
call makesamp mdi
call makesamp msgbox
cd opendlg
call ..\makesamp hello
cd ..
call makesamp pmcap
call makesamp snap
call makesamp spy
call makesamp vectfont
call makesamp wmchar
