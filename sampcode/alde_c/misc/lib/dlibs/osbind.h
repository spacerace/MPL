/*
 *	OSBIND.H	Mnemonic names for operating system calls
 */

/*	
 *	GEMDOS	(trap1)
 */
#define	Pterm0()			gemdos(0x00)
#define	Cconin()			gemdos(0x01)
#define	Cconout(c)			gemdos(0x02,c)
#define	Cauxin()			gemdos(0x03)
#define	Cauxout(c)			gemdos(0x04,c)
#define	Cprnout(c)			gemdos(0x05,c)
#define	Crawio(data)			gemdos(0x06,data)
#define	Crawcin()			gemdos(0x07)
#define	Cnecin()			gemdos(0x08)
#define	Cconws(s)			gemdos(0x09,s)
#define	Cconrs(buf)			gemdos(0x0A,buf)
#define	Cconis()			(int)gemdos(0x0B)
#define	Dsetdrv(d)			gemdos(0x0E,d)
#define	Cconos()			gemdos(0x10)
#define	Cprnos()			gemdos(0x11)
#define	Cauxis()			gemdos(0x12)
#define	Cauxos()			gemdos(0x13)
#define	Dgetdrv()			(int)gemdos(0x19)
#define	Fsetdta(dta)			gemdos(0x1A,dta)
#define	Super(ptr)			gemdos(0x20,ptr)
#define	Tgetdate()			(int)gemdos(0x2A)
#define	Tsetdate(date)			gemdos(0x2B,date)
#define	Tgettime()			(int)gemdos(0x2C)
#define	Tsettime(time)			gemdos(0x2D,time)
#define	Fgetdta()			gemdos(0x2F)
#define	Sversion()			(int)gemdos(0x30)
#define	Ptermres(save,rv)		gemdos(0x31,save,rv)
#define	Dfree(buf,d)			gemdos(0x36,buf,d)
#define	Dcreate(path)			gemdos(0x39,path)
#define	Ddelete(path)			gemdos(0x3A,path)
#define	Dsetpath(path)			gemdos(0x3B,path)
#define	Fcreate(fn,mode)		gemdos(0x3C,fn,mode)
#define	Fopen(fn,mode)			gemdos(0x3D,fn,mode)
#define	Fclose(h)			gemdos(0x3E,h)
#define	Fread(h,cnt,buf)		gemdos(0x3F,h,cnt,buf)
#define	Fwrite(h,cnt,buf)		gemdos(0x40,h,cnt,buf)
#define	Fdelete(fn)			gemdos(0x41,fn)
#define	Fseek(where,h,how)		gemdos(0x42,where,h,how)
#define	Fattrib(fn,rwflag,attr)		gemdos(0x43,fn,rwflag,attr)
#define	Fdup(h)				gemdos(0x45,h)
#define	Fforce(hstd,hnew)		gemdos(0x46,hstd,hnew)
#define	Dgetpath(buf,d)			gemdos(0x47,buf,d)
#define	Malloc(size)			gemdos(0x48,size)
#define	Mfree(ptr)			gemdos(0x49,ptr)
#define	Mshrink(ptr,size)		gemdos(0x4A,0,ptr,size)
#define	Pexec(mode,prog,tail,env)	gemdos(0x4B,mode,prog,tail,env)
#define	Pterm(rv)			gemdos(0x4C,rv)
#define	Fsfirst(filespec,attr)		(int)gemdos(0x4E,filespec,attr)
#define	Fsnext()			(int)gemdos(0x4F)
#define	Frename(zero,old,new)		gemdos(0x56,zero,old,new)
#define	Fdatime(timeptr,h,rwflag)	gemdos(0x57,timeptr,h,rwflag)

/*
 *	BIOS	(trap13)
 */
#define	Bconstat(dev)			bios(1,dev)
#define	Bconin(dev)			bios(2,dev)
#define	Bconout(dev,c)			bios(3,dev,c)
#define	Rwabs(rwflag,buf,n,sector,d)	bios(4,rwflag,buf,n,sector,d)
#define	Setexc(vnum,vptr)		bios(5,vnum,vptr)
#define	Tickcal()			bios(6)
#define	Getbpb(d)			bios(7,d)
#define	Bcostat(dev)			bios(8,dev)
#define	Mediach(d)			bios(9,a)
#define	Drvmap()			bios(10)
#define	Getshift()			bios(11,-1)
#define	Kbshift(data)			bios(11,data)

/*
 *	XBIOS	(trap14)
 */
#define	Initmous(type,param,vptr)	xbios(0,type,param,vptr)
#define	Physbase()			xbios(2)
#define	Logbase()			xbios(3)
#define	Getrez()			(int)xbios(4)
#define	Setscreen(lscrn,pscrn,rez)	xbios(5,lscrn,pscrn,rez)
#define	Setpallete(palptr)		xbios(6,palptr)
#define	Setcolor(colornum,mixture)	xbios(7,colornum,mixture)
#define	Floprd(buf,x,d,sect,trk,side,n)	xbios(8,buf,x,d,sect,trk,side,n)
#define	Flopwr(buf,x,d,sect,trk,side,n)	xbios(9,buf,x,d,sect,trk,side,n)
#define	Flopfmt(b,x,d,spt,trk,sd,i,m,v)	xbios(10,b,x,d,spt,trk,sd,i,m,v)
#define	Midiws(cnt,ptr)			xbios(12,cnt,ptr)
#define	Mfpint(vnum,vptr)		xbios(13,vnum,vptr)
#define	Iorec(iodev)			xbios(14,iodev)
#define	Rsconf(baud,flow,uc,rs,ts,sc)	xbios(15,baud,flow,uc,rs,ts,sc)
#define	Keytbl(nrml,shft,caps)		xbios(16,nrml,shft,caps)
#define	Random()			xbios(17)
#define	Protobt(buf,serial,dsktyp,exec)	xbios(18,buf,serial,dsktyp,exec)
#define	Flopver(buf,x,d,sect,trk,sd,n)	xbios(19,buf,x,d,sect,trk,sd,n)
#define	Scrdmp()			xbios(20)
#define	Cursconf(func,rate)		xbios(21,func,rate)
#define	Settime(time)			xbios(22,time)
#define	Gettime()			xbios(23)
#define	Bioskeys()			xbios(24)
#define	Ikbdws(len_minus1,ptr)		xbios(25,len_minus1,ptr)
#define	Jdisint(vnum)			xbios(26,vnum)
#define	Jenabint(vnum)			xbios(27,vnum)
#define	Giaccess(data,reg)		xbios(28,data,reg)
#define	Offgibit(ormask)		xbios(29,ormask)
#define	Ongibit(andmask)		xbios(30,andmask)
#define	Xbtimer(timer,ctrl,data,vnum)	xbios(31,timer,ctrl,data,vnum)
#define	Dosound(ptr)			xbios(32,ptr)
#define	Setprt(config)			xbios(33,config)
#define	Kbdvbase()			xbios(34)
#define	Kbrate(delay,reprate)		xbios(35,delay,reprate)
#define	Prtblk(pblkptr)			xbios(36,pblkptr)
#define	Vsync()				xbios(37)
#define	Supexec(funcptr)		xbios(38,funcptr)