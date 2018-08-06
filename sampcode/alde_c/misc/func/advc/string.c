#include <ctype.h>

int any2dec(str,radix)                /* convert string to integer */
   char *str;
   unsigned int radix;
{
   unsigned int digit, number = 0;
   while (*str) {
      if (isdigit(*str)) digit = *str - '0';
      else if (islower(digit)) digit = *str - 'a' + 10;
      else digit = *str - 'A' + 10;
      number = number * radix + digit;
      str++; }
   return(number);
}



unsigned char *bsqueeze(line)         /* squeeze blanks from a string */
   unsigned char *line;
{
   static unsigned char sqbuf[126];
   unsigned char *sqptr = sqbuf;
   int repcnt = 0;

   while (*line) {
      while(*line == ' ') {
         line++;
         repcnt++; }
      if (repcnt == 2)
         *sqptr++ = ' ' | 128;
      else if (repcnt > 2) {
         *sqptr++ = 128;
         *sqptr++ = repcnt | 128; }
      else if (repcnt) {
         if (sqptr == &sqbuf[0])
            *sqptr++ = ' ';
         else *(sqptr - 1) += 128; }
      repcnt = 0;
      *sqptr++ = *line++; }
   *sqptr = '\0';
   return(&sqbuf[0]);
}



unsigned char *bunsqueeze(line)       /* unsqueeze a blank-squeezed string */
   unsigned char *line;
{
   static unsigned char usqbuf[126];
   unsigned char *usqptr = usqbuf;
   int repcnt = 0;

   while (*line) {
      if (*line < 128)
         *usqptr++ = *line;
      else if (*line == 128) {
         repcnt = *++line - 128;
         while (repcnt--)
             *usqptr++ = ' '; }
      else {
         *usqptr++ = *line - 128;
         *usqptr++ = ' '; }
      line++; }
   *usqptr = '\0';
   return(&usqbuf[0]);
}



char *dec2any(number,radix)           /* convert integer to string */
   unsigned int number, radix;
{
   static char buffer[64];      /* allow for up to 64-bit integers */
   char *bufptr = buffer + 64;  /* point to end of buffer */
   int digit;
   *bufptr-- = '\0';
   do {
      digit = number % radix;
      number /= radix;
      *bufptr-- = (digit < 10) ? (digit + '0') : (digit + 'A' - 10); }
   while (number);
   return(++bufptr);
}



char *extract(str,delim,element)    /* extract element from delimited string */
   char *str, delim;
   int element;
{
   static char elembuf[80];
   char *elem = elembuf, *temp;

   temp = str;
   while (*str && element)
      if (*str++ == delim)
         if (--element) temp = str;
   if ((element == 1) && (*str == '\0') || (element == 0))
      while (*temp && (*temp != delim))
         *elem++ = *temp++;
   *elem = '\0';
   return(&elembuf[0]);
}



char *locase(str)                     /* convert string to lowercase */
   char *str;
{
   char *start;
   start = str;
   while (*str) {
      if (isupper(*str))
         *str = tolower(*str);
      str++; }
   return(start);
}



char *multiand(str,c)                 /* AND string with a given value */
   char *str, c;
{
   char *start;

   start = str;
   while (*str)
      *str++ &= c;
   return(start);
}



char *multior(str,c)                  /* OR string with a given value */
   char *str, c;
{
   char *start;

   start = str;
   while (*str)
      *str++ |= c;
   return(start);
}



char *multixor(str,c)                 /* XOR string with a given value */
   char *str, c;
{
   char *start;

   start = str;
   while (*str)
      *str++ ^= c;
   return(start);
}



char *reverse(str)                    /* reverse order of chrs in a string */
   char *str;
{
   char *start, *end, tmp;
   int length;

   end = str + (length = strlen(start = str)) -1;
   length >>= 1;

   while (length--) {
      tmp = *str;
      *str++ = *end;
      *end-- = tmp; }
   return(start);
}



char *soundex(str)                    /* get soundex code for a string */
   char *str;
{
   static char soundexbuf[81];
   char *table = "01230120022455012623010202";
   char *sdx = soundexbuf, lastchr = ' ';

   while (*str) {
      if (isalpha(*str) && (*str != lastchr)) {
         *sdx = *(table + toupper(*str) - 'A');
         if ((*sdx != '0') && (*sdx != lastchr))
            lastchr = *sdx++; }
      str++; }
   *sdx = '\0';
   return(&soundexbuf[0]);
}



char *strip(str,c)                    /* strip a given chr from a string */
   char *str, c;
{
   char *start, *newstr;

   start = newstr = str;
   while (*str) {
      if (*str != c)
         *newstr++ = *str;
      str++; }
   *newstr = '\0';
   return(start);
}



char *striprange(str,clo,chi)         /* strip a range of chrs from a string */
   char *str, clo, chi;
{
   char *start, *newstr;

   start = newstr = str;
   while (*str) {
      if ((*str > chi) | (*str < clo))
         *newstr++ = *str;
      str++; }
   *newstr = '\0';
   return(start);
}



char *upcase(str)                     /* convert a string to uppercase */
   char *str;
{
   char *start;
   start = str;
   while (*str) {
      if (islower(*str))
         *str = toupper(*str);
      str++; }
   return(start);
}
