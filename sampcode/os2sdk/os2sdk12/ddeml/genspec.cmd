
awk "/PUBDOC START/,/PUBDOC END/" *.c *.h | sed -e "s/PUBDOC START//" | sed -e "s/PUBDOC END/----------/" | sed -e "s/^\*//" >spec
