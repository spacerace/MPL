/********************************************************************
 *  This function will output a string to an asynch port.  The baud *
 *  rate divisors and  word  length  should already  be  set.  HXFD *
 *  should point to the line status register and HXF8 should  point *
 *  to the Transmitting Hold Register before calling this  routine. *
 *  If you are using an IBM PC with comm port #1 then these  values *
 *  should be hex 3fd and 3f8 respectively.  If you are using comm2 *
 *  these valute should be 2fd and 2f8.  I  declare HXFD  and  HXF9 *
 *  globally at the first  part of a  program rather  than  passing *
 *  parms all over the place but the routine could be modified very *
 *  easily to handle it in any manner preferred.                    *
 *                                                                  *
 *  This  routine  will  replace one I  earlier put  in the  public *
 *  called "sethayes.c".  If you want to initialize your Hayes then *
 *  pass this function the setup string and the routine will do  it *
 *  for you.  I don't know when I will be able to bring  the  TULSA *
 *  IBBS C-Sig board back on line so those of you who have used the *
 *  board  please be  patient and  hopefully we will  have it  back *
 *  bigger and better than ever before.                             *
 *                     Lynn Long                                    *
 *                     PO Box 471114                                *
 *                     Tulsa, OK 74147-1114                         *
 ********************************************************************/ 
void outstng(outdata)
char *outdata;
{
	unsigned byte = 0x00;
 	while(*outdata != '\0'){
		byte=inp(1021);				/*  3FD in decimal	*/
		byte&=0x20;
		if(byte==0x20){
 			outp(1016,*outdata);	/*  3F8 in decimal	*/
			*outdata++;
		}
    }
 }
