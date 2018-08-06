char *strdup(string)
register char *string;
/*
 *	Create a copy of <string> and return a pointer to the copy.
 */
{
	register char *p;
	char *malloc();

	if(p = malloc(strlen(string) + 1))
		strcpy(p, string);
	return(p);
}
