'
' Test of function ega() and egas() in ega.lib.  Tests all of the parameters
' passed to these two functions.
'
' To make qbega.exe:		QB QBEGA;
'				LINK QBEGA,,,EGA;
'			
'

' Interrogate the driver, get the version number.
'
'
    e1% = &h00fa
    e2% = 0
    e3% = 0
    e4% = 0
    e5% = 0
    
    call ega(e1%, e2%, e3%, e4%, e5%)

    if (e2% <> 0) then 100
    print "Mouse Driver not found"
    end
100 print "Mouse Driver found, version ";
    def seg = e5%
    majver = peek(e2%)
    minver = peek(e2%+1)
    def seg
    print " = ";
    print using "#";majver;
    print ".";
    print using "##";minver
    print
    def seg
    
    
' Test the read and write register range functions F2 and F3.  Save old value,
' write new value, read value, restore original value, if value read was not
' the value written, then something is wrong.
'
    
    print "Read/Write Range functions (F2, F3) ";
    
    nvals = 2
    nints = int (nvals/2) + 1
    start = &h0e

    dim orbuf%(1)
    dim rdbuf%(1)
    dim wrbuf%(1)
    
    orbuf%(0) = &haaaa
    rdbuf%(0) = &hbbbb
    wrbuf%(0) = &hcccc
'
    e1%  = &hF2					' read range	
    e3%  = start * 256 + nvals			' start reg & num regs
    e4%  = 0					' crt controller
    e5%  = 0					' dummy arg

    call ega(e1%, orbuf%(0), e3%, e4%, e5%)
    
    wrbuf%(0) = &heeff
    e1% = &hF3					' write range
    call ega(e1%, wrbuf%(0), e3%, e4%, e5%)
    
    
    e1% = &hF2					' read range
    call ega(e1%, rdbuf%(0), e3%, e4%, e5%)
    
    e1% = &hF3					' write range
    call ega(e1%, orbuf%(0), e3%, e4%, e5%)
    
    if (rdbuf%(0) <> wrbuf%(0))  then print "FAILED" else print "PASSED"


'
' Now test segment:offset calls
'
    print
    print
    print "Testing calls with segment:offset, using CALLS and EGAS"
    print

    e1% = &h00fa
    e2% = 0
    e3% = 0
    e4% = 0
    e5% = 0
    
    calls egas(e1%, e2%, e3%, e4%, e5%)

    if (e2% <> 0) then 200
    print "Mouse Driver not found"
    end
200 print "Mouse Driver found, version ";
    def seg = e5%
    majver = peek(e2%)
    minver = peek(e2%+1)
    def seg
    print " = ";
    print using "#";majver;
    print ".";
    print using "##";minver
    print
    def seg
    
    print "Read/Write Range functions (F2, F3) ";
    ' read the cursor location high and low registers 
    
    
    nvals = 2
    nints = int (nvals/2) + 1
    start = &h0e

    orbuf%(0) = &haaaa
    rdbuf%(0) = &hbbbb
    wrbuf%(0) = &hcccc
'
    e1%  = &hF2					' read range	
    e3%  = start * 256 + nvals			' start reg & num regs
    e4%  = 0					' crt controller
    e5%  = 0					' dummy arg

    calls egas(e1%, orbuf%(0), e3%, e4%, e5%)
    
    wrbuf%(0) = &heeff
    e1% = &hF3					' write range
    calls egas(e1%, wrbuf%(0), e3%, e4%, e5%)
    
    
    e1% = &hF2					' read range
    calls egas(e1%, rdbuf%(0), e3%, e4%, e5%)
    
    e1% = &hF3					' write range
    calls egas(e1%, orbuf%(0), e3%, e4%, e5%)
    
    if (rdbuf%(0) <> wrbuf%(0))  then print "FAILED" else print "PASSED"
    print

' Thats all

    end
