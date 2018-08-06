#include <stdio.h>
#include <ctype.h>

#define NestMax 10

char CR = 0x0D;
char IDChar[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyz_";
char TINCLUDE[] = "#include";
char *Reserved[] = {
   "auto",
   "break",
   "case",
   "char",
   "continue",
   "default",
   "do",
   "double",
   "else",
   "entry",
   "extern",
   "float",
   "for",
   "goto",
   "if",
   "int",
   "long",
   "register",
   "return",
   "short",
   "sizeof",
   "static",
   "struct",
   "switch",
   "typedef",
   "union",
   "unsigned",
   "void",
   "while"
};

typedef struct
{
   int AX, BX, CX, DX, SI, DI;
} RegPack;

typedef struct
{
   FILE *ID;
   char Name[64];
   int Lnno;
   char InsertChar;
   int EndFile, First, Print;
} FileDef;

typedef struct LR
{
   struct LR *Next;
   int Key;
   char Code;
} LineRec;

typedef struct
{
   LineRec *First, *Current;
} LineQueueRec;

typedef struct
{
   char *Key;
   LineQueueRec *LineList;
} WordRec;

typedef struct TR
{
   struct TR *Left, *Right;
   WordRec *Ref;
} TreeRec;

int PageNum, PageLn;
TreeRec *Root;
FileDef MainFile, IncludeFile, InFile;
FILE *Output;
int TotalLn, ChIndex;
int Listing, StdFlag, IncludeFlag;
int NestUp, NestDn, NestLvl;
int Tabs;
char Token[30], Path[64], IncludePath[64];
int i;
char *FDate, *FTime;
char Today[30];
char *OutFileName[64];
char Ch;
int UnGetCount = 0;
char UnGot = ' ';
char Line[255];

main(argc, argv)
int argc;
char *argv[];
{
   void Init(), GetFileDate(), Header(), GetLiteral(), GetIncludeFile();
   void GetCharLiteral(), GetComment(), GetToken(), PrintLine(), AddQueue();
   void Concordance(), Usage();
   FILE *fopen();
   char GetChar(), *index();

   Init();
   if (argc < 2)
      Usage();
   else
   {
      for (i = 1; i < argc; i++)
      {
         strcpy(Token, argv[i]);
         if (Token[0] == '-')
         {
            UpperCase(Token);
            if (strcmp(Token, "-NL") == 0)
               Listing = FALSE;
            else if (strcmp(Token, "-NS") == 0)
               StdFlag = FALSE;
            else if (strcmp(Token, "-NI") == 0)
               IncludeFlag = FALSE;
            else if (Token[1] == 'T')
            {
               if (i + 1 < argc)
                  Tabs = atoi(argv[++i]);
               else
                  fprintf(stderr, "Invalid Tab option\n");
            }
            else if (Token[1] == 'I')
            {
               if (i + 1 < argc)
               {
                  strcpy(IncludePath, argv[++i]);
                  if (IncludePath[strlen(IncludePath) - 1] != "\\")
                     strcat(IncludePath, "\\");
               }
               else
                  fprintf(stderr, "Invalid Include Path option\n");
            }
            else
            {
               fprintf(stderr, "Invalid option: %s\n", argv[i]);
            }
         }
         else if (strlen(InFile.Name) == 0)
            strcpy(InFile.Name, argv[i]);
         else
         if (strlen(OutFileName) == 0)
            strcpy(OutFileName, argv[i]);
         else
         {
            fprintf(stderr, "Invalid parameter or too many file names: %s\n", argv[i]);
            Abort();
         }
      }
   }
   if (strlen(InFile.Name) == 0)
   {
      fprintf(stderr, "I have nothing to do!  No input file specified.\n");
      Abort();
   }
   if (index(InFile.Name, '.') == NULL)
      strcat(InFile.Name, ".C");
   for (i = strlen(InFile.Name); i >= 0; i--)
   {
      if ((InFile.Name[i] == '\\') || (InFile.Name[i] == '/') || (InFile.Name[i] == ':'))
         break;
   }
   if (i < 0)
      strcpy(Path, "");
   else
   {
      strncpy(Path, InFile.Name, i + 1);
      Path[i + 1] = '\0';
   }
   if (strlen(OutFileName) == 0)
   {
      strcpy(OutFileName, InFile.Name);
      *index(OutFileName, '.') = 0x00;
   }
   if (index('.', OutFileName) == NULL)
      strcat(OutFileName, ".LST");
   if ((InFile.ID = fopen(InFile.Name, "r")) == NULL)
   {
      fprintf(stderr, "I can't find input file: %s\n", InFile.Name);
      Abort();
   }
   InFile.Lnno = 0;
   InFile.EndFile = FALSE;
   InFile.Print = TRUE;
   InFile.First = TRUE;
   if ((Output = fopen(OutFileName, "w")) == NULL)
   {
      fprintf(stderr, "I can't create output file: %s\n", OutFileName);
      Abort();
   }
   MainFile = InFile;
   ChIndex = 0;
   GetFileDate(InFile.ID->_file, &FDate, &FTime);
   strcpy(Today, " Date: ");
   strcat(Today, FDate);
   strcat(Today, " @ ");
   strcat(Today, FTime);

   if (Listing)
      Header();
   while (!MainFile.EndFile)
   {
      while (!InFile.EndFile)
      {
         Ch = GetChar();
         if (Ch == '"')
            GetLiteral();
         else if ((Ch == '#') && IncludeFlag)
            GetIncludeFile();
         else if (Ch == '\'')
            GetCharLiteral();
         else if (Ch == '/')
            GetComment();
         else if (index(IDChar, Ch) != NULL)
            GetToken();
         else if (Ch == '{')
         {
            if (NestLvl == NestMax)
               fprintf(stderr, "---- Too many levels\n");
            else
            {
               NestLvl++;
               NestUp = TRUE;
            }
         }
         else if (Ch == '}')
         {
            if (NestLvl == 0)
               fprintf(stderr, "---- Nesting Error");
            else
            {
               NestLvl--;
               NestDn = TRUE;
            }
         }
      }
      PrintLine();
      if (InFile.InsertChar != ' ')
      {
         fclose(InFile.ID);
         InFile = MainFile;
         ChIndex = 0;
      }
      else
         MainFile = InFile;
   }
   Header();
   fprintf(stderr, "\n");
   fprintf(stderr, "Writing Cross Reference\n");
   Concordance(Root);
   fclose(Output);
   fprintf(stderr, "    Total Pages= %d\n", PageNum);
}

void BackCh(Ch)
char Ch;
{
   UnGot = Ch;
   UnGetCount = 1;
}

char GetChar()
{
   char Ch, InLine[255];
   int i, j;

   if (UnGetCount)
   {
      UnGetCount = 0;
      return (UnGot);
   }
   if (InFile.First)
   {
      InFile.ID = fopen(InFile.Name, "r");
      InFile.First = FALSE;
   }
   if (ChIndex == 0)
   {
      if (!InFile.EndFile)
      {
         if (InFile.Print)
         {
            InFile.Print = FALSE;
            fprintf(stderr, "\n");
            fprintf(stderr, "Line= %4d%c Total Lines= %4d  ", InFile.Lnno, InFile.InsertChar, TotalLn);
            if (InFile.InsertChar != ' ')
               fprintf(stderr, "%13s", "");
            fprintf(stderr, "%s", InFile.Name);
            fputc(CR, stderr);
         }
         else
            PrintLine();
         fgets(InLine, 255, InFile.ID);
         i = j = 0;
         while (InLine[i])
         {
            if (InLine[i] == '\t')
            {
               do
                  Line[j++] = ' ';
               while (j % Tabs);
            }
            else
               Line[j++] = InLine[i];
            ++i;
         }
         Line[j] = 0x00;
         if (feof(InFile.ID))
            InFile.EndFile = TRUE;
         else
         {
            Line[strlen(Line) - 1] = 0x00;
            InFile.Lnno++;
            TotalLn++;
            GotoXY(6, WhereY());
            fprintf(stderr, "%4d%c", InFile.Lnno, InFile.InsertChar);
            GotoXY(25, WhereY());
            fprintf(stderr, "%4d", TotalLn);
            NestUp = FALSE;
            NestDn = FALSE;
            ChIndex = 1;
         }
      }
   }
   if (!InFile.EndFile)
   {
      if (ChIndex <= strlen(Line))
         Ch = Line[ChIndex++ - 1];
      else
      {
         Ch = ' ';
         ChIndex = 0;
      }
   }
   return (Ch);
}

void Init()
{
   printf("C Cross Reference: Version 2.00  June 25, 1986\n\n");
   PageNum = 0;
   Root = NULL;
   InFile.InsertChar = ' ';
   TotalLn = 0;
   IncludeFile.InsertChar = '@';
   Listing = TRUE;
   StdFlag = TRUE;
   IncludeFlag = TRUE;
   NestUp = FALSE;
   NestDn = FALSE;
   NestLvl = 0;
   Tabs = 4;
   IncludePath[0] = 0x00;
}

int UpperCase(Str)
char *Str;
{
   int i;

   for (i = 0; i < strlen(Str); i++)
      if (islower(Str[i]))
         Str[i] = (char) toupper(Str[i]);
}

void GetFileDate(Handle, Date, Time)
int Handle;
char **Date, **Time;
{
   char *malloc();

   struct
   {
      int AX, BX, CX, DX, SI, DI;
   } Regs;

   Regs.AX = 0x5700;
   Regs.BX = Handle;
   intdos(&Regs, &Regs);
   *Date = malloc(9);
   sprintf(*Date, "%02d-%02d-%02d", ((Regs.DX >> 5) & 0x0F), (Regs.DX & 0x1F),
           (Regs.DX >> 9));
   *Time = malloc(9);
   sprintf(*Time, "%02d:%02d:%02d", (Regs.CX >> 11), ((Regs.CX >> 5) & 0x3F),
           (Regs.CX & 0x1F));
}

void Header()
{
   PageNum++;
   fputc(0x0C, Output);                /* Form Feed */
   fprintf(Output, "%-48s", MainFile.Name);
   fprintf(Output, "Block Structure and Cross Reference");
   fprintf(Output, "%29s   Page %3d", Today, PageNum);
   fprintf(Output, "\n\n\n");
   PageLn = 60;
}

void GetLiteral()
{
   char Ch;

   do
   {
      Ch = GetChar();
      if (Ch == '\\')
      {
         Ch = GetChar();
         Ch = GetChar();
      }
   }
   while ((!InFile.EndFile) && (Ch != '"'));
}

void GetIncludeFile()
{
   extern int stricmp();
   void PrintLine(), GetToken();

   int i, OK;
   char DeLim;
   char Temp[64];

   GetToken();
   if (stricmp(Token, TINCLUDE) == 0)
   {
      OK = TRUE;
      while ((!InFile.EndFile) && (Ch == ' '))
         Ch = GetChar();
      switch (Ch)
      {
      case '"':
         DeLim = Ch;
         break;
      case '<':
         DeLim = '>';
         break;
      default:
         DeLim = ' ';
      }
      for (i = 0; ((!InFile.EndFile) && ((Ch = GetChar()) != DeLim)); i++)
      {
         Temp[i] = Ch;
      }
      Temp[i] = 0x00;
      if (InFile.InsertChar == 'Z')
      {
         fprintf(stderr, "Too many include files\n");
         OK = FALSE;
      }
      else
      {
         if (DeLim == '"')
         {
            strcpy(IncludeFile.Name, Path);
            strcat(IncludeFile.Name, Temp);
         }
         else
         {
            strcpy(IncludeFile.Name, IncludePath);
            strcat(IncludeFile.Name, Temp);
         }
         IncludeFile.InsertChar++;
         IncludeFile.Lnno = 0;
         IncludeFile.EndFile = FALSE;
         IncludeFile.Print = TRUE;
         IncludeFile.First = TRUE;
         if ((IncludeFile.ID = fopen(IncludeFile.Name, "r")) == NULL)
         {
            fprintf(stderr, "\nCan't find include file: %s\n", IncludeFile.Name);
            OK = FALSE;
         }
      }
      PrintLine();
      if (OK)
      {
         MainFile = InFile;
         MainFile.Print = TRUE;
         InFile = IncludeFile;
         ChIndex = 0;
      }
   }
}

void PrintLine()
{
   void FillLine();

   int Column;

   if (Listing)
   {
      fprintf(Output, "%4d%c    ", InFile.Lnno, InFile.InsertChar);
      for (Column = 0; Column < NestLvl - 1; Column++)
         fprintf(Output, "|  ");
      if (NestLvl > 0)
      {
         if (NestUp || NestDn)
         {
            if (NestDn)
            {
               fprintf(Output, "|  ");
               fprintf(Output, "E--");
               for (Column = NestLvl + 1; Column < NestMax; Column++)
                  fprintf(Output, "---");
            }
            else
            {
               fprintf(Output, "B--");
               for (Column = NestLvl; Column < NestMax; Column++)
                  fprintf(Output, "---");
            }
            FillLine(Line);
         }
         else
         {
            fprintf(Output, "|  ");
            for (Column = NestLvl; Column < NestMax; Column++)
               fprintf(Output, "   ");
         }
      }
      else if (NestDn)
      {
         fprintf(Output, "E--");
         for (Column = 1; Column < NestMax; Column++)
            fprintf(Output, "---");
         FillLine(Line);
      }
      else
         for (Column = 0; Column < NestMax; Column++)
            fprintf(Output, "   ");
      fprintf(Output, "%s\n", Line);
      PageLn--;
      if (PageLn <= 0)
         Header();
   }
}

void FillLine(Line)
char *Line;
{
   int i;

   for (i = 0; i < strlen(Line) && Line[i] == ' '; i++)
      Line[i] = '-';
}


void GetToken()
{
   TreeRec *BinaryTree();

   int j;

   j = 0;
   do
   {
      Token[j] = Ch;
      j++;
      Ch = GetChar();
   }
   while ((index(IDChar, Ch) != NULL) && (!InFile.EndFile));
   Token[j] = 0x00;
   if (StdFlag)
      Root = BinaryTree(Root);
   else if (!BinarySearch(Token, Reserved, 29))
      Root = BinaryTree(Root);
}

void GetComment()
{
   char Ch;

   if (!InFile.EndFile)
      Ch = GetChar();
   if ((!InFile.EndFile) && (Ch == '*'))
   {
      while ((!InFile.EndFile) && (Ch != '/'))
      {
         while ((!InFile.EndFile) && (Ch != '*'))
            Ch = GetChar();
         Ch = GetChar();
      }
   }
   else
      BackCh(Ch);
}

void GetCharLiteral()
{
   char Ch;

   do
   {
      Ch = GetChar();
      if (Ch == '\\')
      {
         Ch = GetChar();
         Ch = GetChar();
      }
   }
   while ((!InFile.EndFile) && (Ch != '\''));
}

int BinarySearch(T, A, Num)
char *T, *A[];
int Num;
{
   extern int stricmp();
   int i, j, k, Result;

   i = 0;
   j = Num;
   do
   {
      k = (i + j) / 2;
      Result = stricmp(A[k], T);
      switch (Result)
      {
      case -1:
         i = ++k;
         break;
      case 1:
         j = --k;
         break;
      default:
         break;
      }
   }
   while ((Result != 0) && (i <= j));
   if (Result == 0)
      return (TRUE);
   else
      return (FALSE);
}

TreeRec *BinaryTree(wl)
TreeRec *wl;
{
   extern int stricmp();

   TreeRec *w;

   w = wl;
   if (w == NULL)
   {
      w = (TreeRec *) malloc(sizeof(*w));
      w->Ref = (WordRec *) malloc(sizeof(*(w->Ref)));
      w->Left = w->Right = NULL;
      wl = w;
      w->Ref->Key = malloc(strlen(Token) + 1);
      strcpy(w->Ref->Key, Token);
      w->Ref->LineList = (LineQueueRec *) malloc(sizeof(*(w->Ref->LineList)));
      w->Ref->LineList->First = NULL;
      AddQueue(w->Ref->LineList);
   }
   else
   {
      switch (stricmp(Token, w->Ref->Key))
      {
      case -1:
         w->Left = BinaryTree(w->Left);
         break;
      case 1:
         w->Right = BinaryTree(w->Right);
         break;
      case 0:
         AddQueue(w->Ref->LineList);
         break;
      }
   }
   return (w);
}


void AddQueue(Queue)
LineQueueRec *Queue;
{
   LineRec *w;

   w = (LineRec *) malloc(sizeof(LineRec));
   if (Queue->First == NULL)
      Queue->First = w;
   else
      Queue->Current->Next = w;
   Queue->Current = w;
   w->Next = NULL;
   w->Key = InFile.Lnno;
   w->Code = InFile.InsertChar;
}

char *FillString(Str, Size, Char)
char *Str, Char;
int Size;
{
   char Temp[255];
   int l;

   strcpy(Temp, Str);
   if (strlen(Temp) < Size)
   {
      strcat(Temp, " ");
      l = strlen(Temp);
      while (l < Size)
         Temp[l++] = Char;
      Temp[l] = 0x00;
   }
   return (Temp);
}

void Concordance(T)
TreeRec *T;
{
   char *FillString();

   static char OldCh = 'A';
   LineRec *P;
   int i;

   if (T != NULL)
   {
      Concordance(T->Left);
      if (toupper(T->Ref->Key[0]) != OldCh)
      {
         fprintf(Output, "\n");
         PageLn--;
         if (PageLn <= 0)
            Header();
         OldCh = toupper(T->Ref->Key[0]);
      }
      fprintf(Output, "%s ", FillString(T->Ref->Key, 26, '.'));
      if (!BinarySearch(T->Ref->Key, Reserved, 29))
         fprintf(Output, "%c", ' ');
      else
         fprintf(Output, "%c", 'R');
      P = T->Ref->LineList->First;
      i = 0;
      while (P != NULL)
      {
         if (i >= 17)
         {
            fprintf(Output, "\n");
            i = 0;
            PageLn--;
            if (PageLn <= 0)
               Header();
            fprintf(Output, "%28s", " ");
         }
         fprintf(Output, "%4d%c", P->Key, P->Code);
         i++;
         if (i < 17)
            fprintf(Output, "%s", " ");
         P = P->Next;
      }
      if (i > 0)
      {
         fprintf(Output, "\n");
         PageLn--;
         if (PageLn <= 0)
            Header();
      }
      Concordance(T->Right);
   }
}

void Usage()
{
   fprintf(stderr, "USAGE: cxref input_file_spec [output_file_spec] [options]\n\n");
   fprintf(stderr, "\t-NL\tSuppress production of listing\n");
   fprintf(stderr, "\t-NS\tSuppress cross reference of C key words\n");
   fprintf(stderr, "\t-NI\tSuppress analysis of #include files\n\n");
   fprintf(stderr, "\t-I <path> Provide path for #include <file> files\n");
   fprintf(stderr, "\t-T <n>    Provide the tab stop value for tab expansion\n");
   exit();
}
