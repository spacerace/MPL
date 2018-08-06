/* switch.c	      switch display monitors
		      by Earl C. Terwilliger Jr.
		      05-04-1983
*/
main()
{
  struct regval { int ax,bx,cx,dx,si,di,ds,es; };
  struct regval srv,rrv;

  srv.cx = 0X0F00;			       /* remove the cursor */
  srv.ax = 0X0100;			       /* set cursor type   */
  sysint(16,&srv,&rrv);

  srv.ax = 0X0600;			       /* scroll & blank    */
  srv.cx = 0X0000;			       /* top left	    */
  srv.dx = 0X184F;			       /* bottom right	    */
  srv.bx = 0X0700;			       /* fill attribute    */
  sysint(16,&srv,&rrv);

  sysint(17,&srv,&rrv); 		       /* equipment flag    */
  rrv.bx = rrv.ax;			       /* copy it	    */
  rrv.bx &= 0X0030;			       /* Mask for video    */
  rrv.ax ^= rrv.bx;
  if (rrv.bx == 0X0030) {
   rrv.ax |= 0X0020;
   srv.ax = 0X0002;
  }
  else { rrv.ax |= 0X0030;
	 srv.ax = 0X0007;
  }
  pokeb(0X0410,0,rrv.ax);		       /* save new equip    */
  sysint(16,&srv,&rrv); 		       /* Screen back on    */
}
