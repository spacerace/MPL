sample.obj : sample.c sample.h
     cl -c -Gsw -W2 -Zdp sample.c

sample.res : sample.rc sample.h
     rc -r sample.rc

sample.exe : sample.obj sample.def sample.res
     link4 sample, /align:16, /map /line, slibw, sample
     rc sample.res
     mapsym sample
