lc1 mono -b
lc2 mono -v
masm mhdr;
link mhdr+mono,mono,nul,driver
exe2bin mono.exe mono.sys
del mono.exe
