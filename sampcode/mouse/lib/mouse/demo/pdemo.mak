pdemo.obj:   pdemo.pas
       pas1 pdemo;
       pas2

pdemo.exe:   pdemo.obj subs.obj
       link pdemo subs,,,..\mouse;
