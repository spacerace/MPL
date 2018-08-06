cd %1
for %%x in (*.) do make %%x
del *.obj *.res
cd ..
