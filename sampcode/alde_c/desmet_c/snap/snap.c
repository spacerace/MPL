

/*  Memory snapshot routine written in DeSmet 'C'.
		by Stan Peters, 813 Clara, Palo Alto, 94303.

	A debugging tool, useful for examining structures and file data.
	It is also a valuable learning aid for 'C' but requires that 
	you have (or gain) dump reading ability.            
	It should have good portability, as it was first written in BDS 'C'
	for a CP/M system.                           */

			
			/* test of snap   */

struct stype	{
	char line1[16];
	char line2[16];	  
	char line3[16];
	char *p1;                      
	char *p2;      	  
	char *p3;      
	char *p4;       
} s;

 	char ss1[9];
 	char ss2[9];
 	int  s1;
 	char ss3[9];
 	int  s2;
 	int  s3;
 	char *ss;

main()
{
	for(s1 = 0; s1 <64 ;s1++)
		s.line1[s1] = 1;		/* initialize snap target  */
	strcpy(s.line2,"line2 string");
	snap(s,32,"snap 1");
	strcpy(s.line1,"too long line1 string");
	snap(s,64,"snap 2");
	strcpy (s.line3,s.line1);
	s.p1 = s.line1;
	s.p2 = &s.line1;
	s.p3 = &s.p1;			/* pointer  to string pointer */ 
	s.p4 = *s.p1;			/* illogical, char to ptr	  */
	snap(s,72,"snap 3");

	strcpy(ss1,"1 string");
	strcpy(ss2,"2 string");
	strcpy(ss3,"3 string");
	s1 = 1;   s2 = 2;  s3 = 3;
	ss = &ss1;
	snap(s,64,"snap 4");
}

	/* routine displays memory as hex and characters  */

char stst[80];		/* build space for snap line */

snap(ptr,len,label) 
	char  *ptr;			/* start of area to be snapped */
	int    len;			/* length */
	char  *label;	
{
	char	c;       
	int		g,	h,	i,	j,	k,	l,	pl;
	pl = ptr + len -1;
	printf("\n%s", label);
  for ( l = 0; l < len; l += 16)
  {
	for (g = 4; g < 80; g++)
		stst[g] = ' ';					/* clear line */
	sprintf(stst,"\n%04x    ",ptr);		/* address of data  */
	i = 0;	j = 7;	k = 51;
	for ( g=0; g<4; g++)
	{
		for ( h=0; h < 4 && pl >= ptr; h++)
		{
			c = *ptr++;
			stst[j++] = makeasci(( c >> 4) & 0xf); /* left nibble */
			stst[j++] = makeasci( c & 0xf);       /* right nibble */
			if (( c < ' ') || (c > 0x7e))	/* now character representation */
				c = '.';					/* . if unprintable	*/
			stst[k++] = c;					/* print it */
 		}
 		j += 2;								/* space after every four */
		/*	k++;		insert this to space rt side chars */
	}
	stst[78] = 0;
	puts(stst);
  }
}

int 	makeasci(c)  
		short	c;
 		{
		return (( c > 9 ) ? c + 'A' - 10 : c + '0');
		}
