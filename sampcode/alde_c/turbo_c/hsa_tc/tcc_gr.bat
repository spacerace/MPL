Rem Build HSA_GRAF Demo - Turbo C Compiler
tcc -Ic:\turboc\include -Lc:\turboc\lib gr_demo s_tc_hsa.lib
if errorlevel 1 goto done
gr_demo
:done
