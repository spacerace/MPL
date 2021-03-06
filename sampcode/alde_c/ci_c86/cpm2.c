/*-------------------------------------------------------------------*/
/*                                                                   */
/*  Critical Path Method Project Scheduler - 18 Sept 83 - E. Dong    */
/*  Based on BASIC version, "Hard Hat Management: Two On-Site        */
/*  Tools", by Richard Parry, INTERFACE AGE, February, 1981.         */
/*                                                                   */
/*  Translated into CIC86 "C" by Edward V. Dong                      */
/*                                                                   */
/*  Modifications: (1) No PERT option.                               */
/*                 (2) Added GANTT chart output.                     */
/*                 (3) Allow naming of activities.                   */
/*                                                                   */
/*  Updated 30 Sept 83  - Fix Gantt chart output.                    */
/*                                                                   */
/*-------------------------------------------------------------------*/

#include "stdio.h"
#define  BEGIN  {
#define  END    }
#define  SIZE   100

int i, j, k, m, num;                    /* Internal Integer Parms    */

int c1, c2, du, l1, m1, m2, pl;

int ml[SIZE], mo[SIZE], mp[SIZE], cp[SIZE], me[SIZE], sd[SIZE], ic[20];
int s[SIZE], f[SIZE], d[SIZE], e[SIZE], l[SIZE], f1[SIZE];

char jobname[SIZE][9];				/* Job names */

main()

BEGIN
printf("CPM: Project Scheduler\n\n");		/* identify program */

/* initialize variables */
c1 = 0; c2 = 0; du = 0; m1 = 0; m2 = 0; pl = 0;

printf("Enter number of activities: ");		/* get # of activities*/
scanf("%d",&num);

for (i=1; i<num+1; i++)				/* input routine */
	BEGIN
	printf("Activity %d\n",i);
	getdata();
	printf("\n");
	END

printf("\n--> SORTING IN PROGRESS <--\n");	/* call sort routine */
sort();

printf("Activity #\tName\t");              	/* display sort */
printf("From\tTo\tDuration\n");			/* display sort */
for (i=1; i<num+1; i++)
	BEGIN
	printf("%d\t\t%s\t%d\t%d\t%d\n",i,jobname[i],s[i],f[i],d[i]);
	END

cpm(); 				/* do critical path analysis */
c2 = 0;
printf("\nCP Analysis is:\n");
printf("[EST   = Earliest Starting Time]\n");
printf("[LFT   = Latest  Finishing Time]\n");
printf("[FLOAT = Float  or  Slack  Time]\n");
printf("Name\tFrom\tTo\tEST\tLFT\tFLOAT\n");
for (i=1; i<num+1; i++)
	BEGIN
	printf("%s\t",jobname[i]);
	printf("%d\t%d\t%d\t%d\t%d\n",s[i],f[i],e[s[i]],l[f[i]],f1[i]);
	END
printf("\nThe Critical Path Length is %d\n",pl);
printf("\nThe Critical Path is:\nName\tFrom\tTo\n");
i=1; while (f1[i] != 0) i++;	/* locate start of critical path */
cplout();			/* critical path printout */
gantt();			/* display GANTT chart */
END

/* Sort Data Subroutine, Using Start Node as Key */
sort()
BEGIN
int sw, temp;
sw = 0;
for (i=1; i<num; i++)
	BEGIN
	j = i + 1;
	if (s[i] > s[j])
		BEGIN
		temp = s[i]; s[i] = s[j]; s[j] = temp;
		temp = f[i]; f[i] = f[j]; f[j] = temp;
		temp = d[i]; d[i] = d[j]; d[j] = temp;
		sw = 1;
		END
	END
if (sw==1) sort();	/* sort again if needed */
END

cpm()
BEGIN
printf("--> CPM ANALYSIS IN PROGRESS <--\n");

/* compute earliest starting time */
c1 = 0; c2 = 0; pl = 0;
for (i=1; i<num+1; i++)
	BEGIN
	m1 = e[s[i]] + d[i];
	if (e[f[i]] <= m1) e[f[i]] = m1;
	END

/* compute latest finishing time */
l[f[num]] = e[f[num]];
for (i=num; i>0; i--)
	BEGIN
	l1 = s[i]; m2 = l[f[i]] - d[i];
	if ((l[l1] >= m2) || (l[l1] == 0))
		l[l1] = m2;
	END

/* compute float time */
for (i=1; i<num+1; i++)
	BEGIN
	f1[i] = l[f[i]] - e[s[i]] - d[i];
	if (f1[i] == 0) c1 = c1 + 1;
	END

/* compute critical path length */
for (i=1; i<num+1; i++)
	BEGIN
	if (l[f[i]] > pl) pl = l[f[i]];
	END

/* compute critical path */
i = 0;
while (f[i] != 0) i++;
cpm_test:
	c2 = c2 + 1; cp[i] = cp[i] + 1;
	if (i > num) goto cpm_exit;
for (m=1; m<num+1; m++)
	BEGIN
	if (s[m] != f[i]) continue;
	else if (f1[m] != 0) continue;
		else
			BEGIN
			i=m;
			goto cpm_test;
			END
	END
cpm_exit:
	if (c1 != c2) du = du + 1;
END

/* Input Data Subroutine */
getdata()
BEGIN
	int temp;
	printf("Jobname:\t\t");	scanf("%s",jobname[i]);
	printf("From:\t\t"); 	scanf("%d",&temp); s[i] = temp;
	printf("To:\t\t"); 	scanf("%d",&temp); f[i] = temp;
	printf("Duration:\t"); 	scanf("%d",&temp); d[i] = temp;
	while (f[i] > num)
		BEGIN
		printf("*** End Node # %d ",f[i]);
		printf("Not =< # (%d) of Activities ***\n",num);
		printf("To: ");
		scanf("%d",f[i]);
		END
	if (s[i] >= f[i])
		BEGIN
		printf("*** Start Node (%d)",s[i]);
		printf("Must Be < End Node (%d) ***\n",f[i]);
		printf("\n*** Redo Entry ***\n");
		getdata();
		END
END

/* critical path printout */
cplout()
BEGIN
int iname, jname;
 
iname = s[i];
jname = f[i];
printf("%s\t%d\t%d\n",jobname[i],iname,jname);
c2 = c2 + 1;
if (i<=num)
	BEGIN
	for (m=1; m<num+1; m++)
		BEGIN
		if ((s[m]==f[i]) && (f1[m]==0))
			BEGIN
			i = m;
			cplout();
			END
		END
	END
if (c1 != c2) printf("There is more than one critical path.\n");
END

/* generate GANTT chart */
gantt()
BEGIN

float linelen, scale, xtemp;
int limit, incrm, step;
char newpg, symbol;

printf("%cGANTT Chart for Project:\n\n",newpg);
printf("Non-critical path is denoted as ==\n");
printf("    Critical path is denoted as **\n\n");

/* calculate chart scaling */
linelen = 60.0;
scale = linelen/pl;
incrm = scale;
step=1;
while (incrm < 1)
	BEGIN
	limit = 10.0 * scale;
	step = 60 / limit;
	incrm = limit;
	END

/* generate ruler line for chart */
printf("               0");
for (j=0; j<pl+step; j=j+step)
	BEGIN
	for (k=0; k<incrm; k++)
		BEGIN
		printf("-");
		END
	printf("+");
	END
printf("\n");

/* generate GANTT waterfall */
for (j=1; j<num+1; j++)
	BEGIN
	printf("%8s\t",jobname[j]);
	xtemp = e[s[j]] * scale;
	limit = (xtemp + 0.5);
	for (k=0; k<limit; k++)
		BEGIN
		printf(" ");
		END
	if (f1[j]==0) symbol = '*';
		else  symbol = '=';
	xtemp = l[f[j]] * scale - xtemp;
	limit = (xtemp + 0.5);
	for (k=0; k<limit; k++)
		BEGIN
		printf("%c",symbol);
		END
	printf("\n");
	END
END
                  