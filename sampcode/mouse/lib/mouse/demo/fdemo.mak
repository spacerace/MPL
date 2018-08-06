fdemo.obj:   fdemo.for
       for1 fdemo;
       pas2

fdemo.exe:   fdemo.obj subs.obj
       link fdemo subs,,,..\mouse;
