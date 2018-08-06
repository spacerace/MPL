cd %1
for %%x in (*.) do make %%x
del *.obj
del *.res
cd ..
