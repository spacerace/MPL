{$L QPEX.OBJ}

{ Calls Power2 procedure in QPEX.ASM }

program  Asmtest( input, output );
function POWER2( factor:integer; power:integer ): integer; external;
begin
    writeln( '3 times 2 to the power of 5 is ', POWER2( 3, 5 ) );

end.
