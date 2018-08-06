(* Calls assembly module Power2 (defined in PASCAL.ASM)
 * To compile:
 *  PL pasmain.pas pascal.obj
 *)

program Asmtest( input, output );
function Power2( a:integer; b:integer ): integer; extern;
begin
    writeln( '3 times 2 to the power of 5 is ', Power2( 3, 5 ) );
end.
