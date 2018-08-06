formouse.obj:	formouse.for
       for1 formouse;
       pas2

formouse.exe:	formouse.obj
       link formouse subs,,,..\mouse;
