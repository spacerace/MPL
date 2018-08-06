muzic.obj: muzic.c
    cl -c -W2 -AS -Gsw -Od -Zped muzic.c

muzic.exe: muzic.obj
    link4 muzic, muzic/align:16, muzic/map/li, slibw, muzic.def
    mapsym muzic
