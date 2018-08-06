' Mouse library call test in Quick Basic V2.0

  DIM cursor% (32), cond% (4)

  cursor%(1) =&hf87f   ' initilize data for function call #9
  cursor%(2) =&h0741
  cursor%(3) =&hcf9f
  cursor%(4) =&h9fc7
  cursor%(5) =&h0f81
  cursor%(6) =&h77be
  cursor%(7) =&h0fce
  cursor%(8) =&h8fdf
  cursor%(9) =&hbfe3
  cursor%(10)=&h9ff9
  cursor%(11)=&hc3f9
  cursor%(12)=&hfc01
  cursor%(13)=&hf7fe
  cursor%(14)=&h0ffe
  cursor%(15)=&hf87e
  cursor%(16)=&hff83
  cursor%(17)=&h0780
  cursor%(18)=&hf8be
  cursor%(19)=&h3060
  cursor%(20)=&h6038
  cursor%(21)=&hf07e
  cursor%(22)=&h8841
  cursor%(23)=&hf031
  cursor%(24)=&h7020
  cursor%(25)=&h401c
  cursor%(26)=&h6006
  cursor%(27)=&h3c06
  cursor%(28)=&h03fe
  cursor%(29)=&h0001
  cursor%(30)=&hf001
  cursor%(31)=&h0781
  cursor%(32)=&h007c


  call chkdrv                 ' Check for driver install

  m1%=0                       ' Function 0
  call mouse(m1%,m2%,m3%,m4%) ' initialize mouse
  if ( M1% = 0 ) then
     print "Microsoft Mouse not found"
     end
  end if

  screen 2                     ' Grahpics screen mode

  m1%=9                        ' Function call 9
  m2%=1
  m3%=1
  m4%=VARPTR(cursor%(1))       ' Pointer to currsor array
  call mouse(m1%, m2%, m3%, m4%)

  print "Mouse cursor will disappear within this area."
  print "Press right button to EXIT..................."

  m1% = 1
  call mouse(m1%,m2%,m3%,m4%)

  m1%=16
  cond%(1) = 0
  cond%(2) = 0
  cond%(3) = 390
  cond%(4) = 25
  m4%=VARPTR(cond%(1))
  call mouse(m1%,m2%,m3%,m4%)

  m2% = 99
  while ( m2% <> 2 )
    m1% = 3
    call mouse(m1%,m2%,m3%,m4%)
  wend

  screen 0

  end
