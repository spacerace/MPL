; stubs.asm - a module of empty stubs to keep the BINDer from
; complaining.
;
; Created by Microsoft Corp. 1986

.286p

	public DOSCREATETHREAD
	public DOSSEMCLEAR
	public DOSSEMREQUEST
	public DOSSEMSET
	public DOSSEMWAIT
	public DOSSLEEP

code	segment byte public 'CODE'
	assume	cs:code

DOSCREATETHREAD:
DOSSEMCLEAR:
DOSSEMREQUEST:
DOSSEMSET:
DOSSEMWAIT:
DOSSLEEP:
	ret

code	ends
	end
