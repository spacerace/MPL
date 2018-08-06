' Mouse library call test in Quick Basic V2.0

  call chkdrv

  screen 0

  m1%=0 					' Function 0
  call mouse(m1%,m2%,m3%,m4%)
  if ( M1% = 0 ) then
     print "Microsoft Mouse NOT found"
     end
  end if

  m1%=4 					' Function 4
  m3%=200
  m4%=100
  call mouse(m1%,m2%,m3%,m4%)

  m1%=7 					' Function 7
  m3%=150
  m4%=450
  call mouse(m1%,m2%,m3%,m4%)

  m1%=8 					' Function 8
  m3%=50
  m4%=150
  call mouse(m1%,m2%,m3%,m4%)

  screen 2

  print "graphics cursor, bounded in center screen"
  print "Press left button to EXIT."

  m1% = 1
  call mouse(m1%,m2%,m3%,m4%)

  m2% = 99
  while ( m2% <> 1 )
    m1% = 3
    call mouse(m1%,m2%,m3%,m4%)
  wend

  screen 0

  end
