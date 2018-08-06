bdemo.obj:   bdemo.bas
       qb bdemo;

bdemo.exe:   bdemo.obj subs.obj
       link bdemo subs,,,..\mouse;
