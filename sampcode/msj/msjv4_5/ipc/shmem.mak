#
# make file	for	shmem.c
#

COPT=/Lp /W3 /Zpiel /G2s /I$(INCLUDE) /Alfw

shmem.exe: shmem.c shmem.mak
	cl $(COPT) shmem.c /link /co /noi llibcmt
	markexe windowcompat shmem.exe







