1
 /***********************************************************************/
 /* CHMOD - EXERCISE CHMOD DOS FUNCTION CALL (#43)                      */
 /***********************************************************************/
 /* (C) COPYRIGHT JERRY M. CARLIN 8/14/1983                             */
 /* PERMISSION GIVEN TO USE OR DISTRIBUTE SO LONG AS NO MONEY CHARGED.  */
 /* IF QUESTIONS, CALL 415-680-7590.                                    */
 /***********************************************************************/
/*
#DEFINE DEBUG 1
*/
#INCLUDE "STDIO.H"
#DEFINE CARRY  1        /* 8086 FLAGS CARRY BIT */
#DEFINE NO_MORE 18      /* DOS 'NO MORE FILES' ERROR CODE */
#DEFINE FCALL  0X21     /* FUNCTION CALL INT PER DOS 2.0 MANUAL PG D-2 */
#DEFINE CHMOD  0X43     /* DOS 2.0 CHANGE FILE MODE PG D-39 */
#DEFINE SETDTA 0X1A     /* SET DISK TRANSFER ADDRESS */
#DEFINE FFIRST 0X4E     /* DOS 2.0 FIND FIRST FILE */
#DEFINE FNEXT  0X4F     /* DOS 2.0 FIND NEXT FILE */
 /* FILE ATTRIBUTES FROM DOS 2.0 MANUAL PAGE C-4 */
#DEFINE RO     01       /* READ ONLY */
#DEFINE HIDE   02       /* HIDDEN FILE */
#DEFINE SYS    04       /* SYSTEM FILE */
#DEFINE VOLLBL 08       /* SPECIAL VOLUME LABEL - IN FIRST 11 BYTES */
#DEFINE SUBDIR 0X10     /* DEFINES A SUBDIRECTORY */
#DEFINE ARCHV  0X20     /* ARCHIVE BIT */
CHAR *INDEX();
STRUCT REGVAL {
        UNSIGNED AX,BX,CX,DX,SI,DI,DS,ES;
} IREGS, OREGS, FILEREGS;
STRUCT SEGREG {
        UNSIGNED CS,SS,SDS,SES;
} SR;
MAIN(AC,AV)
INT AC;
CHAR *AV[];
{
        CHAR ATTR[9];           /* FILE ATTRIBUTES */
        INT FLAGS;              /* 8086 FLAG REGISTER */
        CHAR BUFF[128];         /* DISK I/O BUFFER */
        CHAR FULLNAME[128];     /* FULL FILE NAME INCLUDING DRIVE AND DIR */
        CHAR DDPREFIX[120];     /* DRIVE AND DIRECTORY INFO */
        INT I;
        BDOS(SETDTA,BUFF);      /* SET DATA TRANSFER ADDRESS */
        IREGS.AX = 17152;       /* SET AH = 43 */
        SEGREAD(&SR);           /* GET STACK & DATA SEG REGS */
        IREGS.DS = SR.SDS;
        IREGS.ES = SR.SES;
        FILEREGS.DS = IREGS.DS;
        FILEREGS.ES = IREGS.ES;
        FILEREGS.AX = 0X4E00;                   /* FIND FIRST */
        FILEREGS.CX = HIDE | SYS | SUBDIR;      /* ALLOW ANY FILE */
        IF (STRCMP(AV[1],"HELP") == 0 || STRCMP(AV[1],"?") == 0 || AC == 1)
        {
                PRINTF("\NCHMOD [-[R][H][S][A]] FILENAME.EXT ");
                PRINTF("[FILENAME.EXT...]\N");
                PRINTF("\NUSE THE '-' OPTION TO SET FILE MODES. OMIT");
                PRINTF(" THE '-' OPTION FOR MODE DISPLAY.\NA DASH BY ");
                PRINTF(" ITSELF (CHMOD - FILENAME) SETS NORMAL MODE.\N\N");
                PRINTF("SETTABLE FLAGS:\N  R = READ ONLY\N");
                PRINTF("  H = HIDDEN\N  S = SYSTEM");
                PRINTF("\N  A = ARCHIVE\N\NANY FLAG NOT SET WILL BE RESET.");
                PRINTF(" THIS MEANS THAT IF YOU DO NOT, \NFOR EXAMPLE, SET");
                PRINTF(" READ-ONLY THE FILE WILL BE WRITE ENABLED.\N\N");
                PRINTF("DOS GLOBAL FILENAME CHARS '*' AND '?' MAY BE USED.\N");
                PRINTF("\NMODE DISPLAY INCLUDES 'W' FOR WRITE OK AND ");
                PRINTF("'D' FOR A SUBDIRECTORY.\N\N");
                EXIT(0);
        }
        IF (AV[1][0] == '-')    /* SET MODE */
        {
                IREGS.AX |= 1;  /* SET UPDATE BIT */
                IREGS.CX = 0;
                IF (INDEX(AV[1],'R') != 0)
                        IREGS.CX |= RO;
                IF (INDEX(AV[1],'H') != 0)
                        IREGS.CX |= HIDE;
                IF (INDEX(AV[1],'S') != 0)
                        IREGS.CX |= SYS;
                IF (INDEX(AV[1],'A') != 0)
                        IREGS.CX |= ARCHV;
#IFDEF DEBUG
                PRINTF("FUN CODE =%X\N",IREGS.AX);
                PRINTF("SET MODE =%X\N",IREGS.CX);
#ENDIF
                ++AV;   /* SKIP OVER SET MODE WORD */
                --AC;
                WHILE (--AC > 0)
                {
                        FILEREGS.DX = *++AV;
                        FLAGS = SYSINT(FCALL,&FILEREGS,&OREGS);
                        IF (FLAGS & CARRY)
                        {
                                PERROR(FILEREGS.DX,OREGS.AX);
                                CONTINUE;
                        }
                        BUILDP(*AV,DDPREFIX);
                        WHILE (OREGS.AX != NO_MORE)
                        {
                                STRCPY(FULLNAME,DDPREFIX);
                                STRCAT(FULLNAME,BUFF+30);
                                IREGS.DX = FULLNAME;
                                FLAGS = SYSINT(FCALL,&IREGS,&OREGS);
                                IF (FLAGS & CARRY)
                                        PERROR(IREGS.DX,OREGS.AX);
                                OREGS.AX = BDOS(FNEXT,0) & 0XFF;
                        }
                }
                EXIT(0);
        }
 /**********************************/
 /* ELSE REPORT ON CURRENT SETTING */
 /**********************************/
        WHILE (--AC > 0)
        {
                FILEREGS.DX = *++AV;
                FLAGS = SYSINT(FCALL,&FILEREGS,&OREGS);   /* FIND FIRST */
                IF (FLAGS & CARRY)
                {
                        PERROR(FILEREGS.DX,OREGS.AX);
                        CONTINUE;
                }
                I = 0;
                BUILDP(*AV,DDPREFIX);
                WHILE (I != NO_MORE)
                {
                        STRCPY(FULLNAME,DDPREFIX);
                        STRCAT(FULLNAME,BUFF+30);       /* SEE PG D-49 */
                        IREGS.DX = FULLNAME;            /* NEXT FILE */
                        FLAGS = SYSINT(FCALL,&IREGS,&OREGS);
#IFDEF DEBUG
                        PRINTF("FILE     = %S\N",IREGS.DX);
                        PRINTF("ERR CODE = %X\N",OREGS.AX);
                        PRINTF("8086FLAGS= %X\N",FLAGS);
                        PRINTF("MODE BYTE= %D\N",OREGS.CX);
#ENDIF
                        IF (FLAGS & CARRY)
                        {
                                PERROR(IREGS.DX,OREGS.AX);
                                BREAK;
                        }
                        IF (OREGS.CX & RO)
                                ATTR[0] = 'R';
                        ELSE
                                ATTR[0] = 'W';
                        IF (OREGS.CX & HIDE)
                                ATTR[1] = 'H';
                        ELSE
                                ATTR[1] = '-';
                        IF (OREGS.CX & SYS)
                                ATTR[2] = 'S';
                        ELSE
                                ATTR[2] = '-';
                        IF (OREGS.CX & SUBDIR)
                                ATTR[3] = 'D';
                        ELSE
                                ATTR[3] = '-';
                        IF (OREGS.CX & ARCHV)
                                ATTR[4] = 'A';
                        ELSE
                                ATTR[4] = '-';
                        ATTR[5] = '\0';
                        PRINTF("%S %S\N",ATTR,IREGS.DX);
                        I = BDOS(FNEXT,0) & 0XFF;  /* FIND NEXT */
                }
        }
        EXIT(0);
}
 /*****************************************************************/
 /* PRINT ERROR - SEE ERROR RETURN TABLE DOS 2.0 MANUAL PAGE D-14 */
 /*****************************************************************/
PERROR(S,CODE)
CHAR *S;
UNSIGNED CODE;
{
        PRINTF("CHMOD: FILE %S",S);
        SWITCH(CODE) {
        CASE 2:
        CASE 18: /* PERROR CALLED WITH 18 IF BAD FILENAME ENTERED */
                PRINTF(" NOT FOUND.\N");
                BREAK;
        CASE 3:
                PRINTF(". PATH NOT FOUND.\N");
                BREAK;
        CASE 5:
                PRINTF(". ACCESS DENIED.\N");
                BREAK;
        DEFAULT:
                PRINTF(". DOS ERROR CODE %U RETURNED.\N",CODE);
        }
        RETURN;
}
 /********************************************************/
 /* BUILD PREFIX STR CONTAINING DRIVE AND DIRECTORY INFO */
 /* NEEDED SINCE FFIRST DTA DOES NOT STORE THIS INFO     */
 /********************************************************/
BUILDP(FULLNAME,DDPREFIX)
CHAR *FULLNAME; /* FULL PATH NAME (INPUT) */
CHAR *DDPREFIX; /* DRIVE AND DIRECTORY INFO */
{
#IFDEF DEBUG
        CHAR *SAVEP;
        SAVEP = DDPREFIX;
#ENDIF
        WHILE ((INDEX(FULLNAME,':') != 0) || (INDEX(FULLNAME,'\\') != 0))
                *DDPREFIX++ = *FULLNAME++;
        *DDPREFIX = '\0';
#IFDEF DEBUG
        PRINTF("PREFIX=%S\N",SAVEP);
#ENDIF
}
PRESS ENTER TO CONTINUE: 