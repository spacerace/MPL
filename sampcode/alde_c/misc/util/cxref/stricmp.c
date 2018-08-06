int stricmp(a, b)
char *a, *b;
{
   int Result;

   while (((Result = toupper(*a++) - toupper(*b++)) == 0) && (*(a - 1) != 0));
   if (Result < 0)
      Result = -1;
   if (Result > 0)
      Result = 1;
   return (Result);
}
