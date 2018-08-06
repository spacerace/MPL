/* EXAMPLE OF I/O STATEMENTS */
#include "bc.h"
main() {
char *S = "STRING", *FLNAME = "TDAT.G";
int A = 234, B = 678;
if((fp1=fopen( "DATA.TST", "w"))==0){puts("can't open #1");exit(1);}
fprintf(fp1, "%s %d %d", S, A, B);
fclose(fp1);
if((fp1=fopen( "DATA.TST", "r"))==0){puts("can't open #1");exit(1);}
IEOF = fscanf(fp1, "%s %d %d", S, &A, &B);
fclose(fp1);
printf("%s %d %d \n", S, A, B);
LIST(FLNAME);
}/* IOEX*/

LIST(FILENAME)
char *FILENAME;
{
char C;
if((fp4=fopen( FILENAME, "r"))==0){puts("can't open #4");exit(1);}
while((C = getc(fp4)) != EOF) {
  putchar(C);
}/*WH*/
fclose(fp4);
}/* LIST*/

/* ERATOSTHENES SIEVE */
#include "bc.h"
#define SIZE 8190
main() {
int ITER, COUNT, I, K;
int PRIME, FLAG[8191];
printf("10 ITERATIONS \n");
for(ITER = 1; ITER <= 10; ITER++) {
  COUNT = 0;
  for(I = 0; I <= SIZE; I++) {
    FLAG[I] = 1;
  }/*I*/
  for(I = 0; I <= SIZE; I++) {
    if(FLAG[I] != 0) {
      PRIME = I+I+3;
      K = I + PRIME;
      while(K <= SIZE) {
        FLAG[K] = 0;
        K = K + PRIME;
      }/*WH*/
      COUNT++;
    }/*IF*/
  }/*I*/
}/*ITER*/
printf(" %d PRIMES \n", COUNT);
}/* SIEVE*/

/* EIGHT QUEENS CHESS PROBLEM */
#include "bc.h"
int COLFREE[8], X[8];
int UPFREE[15], DOWNFREE[15];
int R, K;
main() {
/* INITIALIZE EMPTY BOARD */
for(K = 0; K <= 7; K++) {
  COLFREE[K] = TRUE;
}/*K*/
for(K = 0; K <= 14; K++) {
  UPFREE[K] = DOWNFREE[K] = TRUE;
}/*K*/
R = -1;
ADDQUEEN();
}/* QUEEN8*/

ADDQUEEN()
{
int C;
R++;
for(C = 0; C <= 7; C++) {
  /* IS SQUARE[R,C] FREE? */
  if(COLFREE[C] && UPFREE[R-C+7] && DOWNFREE[R+C]) {
    /* SET QUEEN ON SQUARE[R,C] */
    X[R] = C;
    COLFREE[C] = UPFREE[R-C+7] = DOWNFREE[R+C] = FALSE;
    if(R == 7) {
      printf("\n CONFIGURATION \n");
      for(K = 0; K <= 7; K++) {
        printf(" %d", X[K]);
      }/*K*/
      exit(1);
    ELSE ADDQUEEN();
    }/*IF*/
    /* REMOVE QUEEN FROM SQUARE[R,C)] */
    COLFREE[C] = UPFREE[R-C+7] = DOWNFREE[R+C] = TRUE;
  }/*IF*/
}/*C*/
R--;
}/* ADDQUEEN*/

/* PRODUCT OF TWO MATRICES OF VARIABLE DIMENSIONS */
#include "bc.h"
#define DLIM 21
main() {
double A[DLIM][DLIM], B[DLIM][DLIM], C[DLIM][DLIM];
int I,J,K, N1,N2,N3;
printf("DIMENSIONS = ");
IEOF = scanf("%d %d %d", &N1, &N2, &N3);
/* GENERATE MATRICES */
for(J = 1; J <= N2; J++) {
  for(I = 1; I <= N1; I++) {
    A[I][J] = (double)(J-I);
  }/*I*/
  for(K = 1; K <= N3; K++) {
    B[J][K] = (double)(J+K);
  }/*K*/
}/*J*/
MATPRI(A,N1,N2);
MATPRI(B,N2,N3);
MULT(A,B,C,N1,N2,N3);
MATPRI(C,N1,N3);
}/* MAIN*/

MULT(E,F,G, L1,L2,L3)
double E[DLIM][DLIM], F[DLIM][DLIM], G[DLIM][DLIM];
int L1, L2, L3;
{
int I,J,K;
for(I = 1; I <= L1; I++) {
  for(K = 1; K <= L3; K++) {
    G[I][K] = 0;
    for(J = 1; J <= L2; J++) {
      G[I][K] = G[I][K]+E[I][J]*F[J][K];
    }/*J*/
  }/*K*/
}/*I*/
}/* MULT*/

MATPRI(A, L1,L2)
double A[DLIM][DLIM]; int L1, L2;
{
int I,J;
printf("\n");
for(I = 1; I <= L1; I++) {
  for(J = 1; J <= L2; J++) {
    printf("%8.3f", A[I][J]);
  }/*J*/
  printf("\n");
}/*I*/
}/* MATPRI*/

/* EXAMPLE USING CONDITIONAL STATEMENTS */
#include "bc.h"
main() {
char  *S = "@$^&*+";
int I;
for(I = 1; I <= 5; I++) {
  if(S[I] == '@') {
    printf("@");
  } else if {
    printf("$");
  } else if {
    printf("^");
  } else {
    printf("NO MATCH");
  }/*IF*/
}/*I*/
}/* CONDIT*/

/* TOWERS OF HANOI */
#include "bc.h"
#define NDISK 64
main() {
MOVE(NDISK, 1, 3, 2);
}/* HANOI*/

MOVE(N, A, B, C)
int N, A, B, C;
{
if(N > 0) {
  MOVE(N-1, A, C, B);
  printf("MOVE A DISK FROM %d TO %d \n", A, B);
  MOVE(N-1, C, B, A);
}/*IF*/
}/* MOVE*/

/* INVERSE AND DETERMINANT OF SYMMETRIC MATRIX */
#include "bc.h"
#define DLIM 31
main() {
double A[DLIM][DLIM],R[DLIM][DLIM],DET,SINV();
int I,J,ND;
printf("ND = ");
IEOF = scanf("%d", &ND);
/* GENERATE ND X ND MATRIX */
for(I = 1; I <= ND; I++) {
  for(J = 1; J <= ND; J++) {
    A[I][J]=1.;
  }/*J*/
  A[I][I]=2.;
}/*I*/
MATPRI(A,ND,ND);
DET=SINV(A,R,ND);
MATPRI(R,ND,ND);
printf("%10.3f\n", DET);
}/*MAIN*/

double SINV(A,R,NN)
double A[DLIM][DLIM], R[DLIM][DLIM];
int NN;
{
double VEC[DLIM],DET,RL;
int I,J,K,L;
DET=A[1][1];
R[1][1]=1./A[1][1];
for(L = 2; L <= NN; L++) {
  K=L-1;
  RL=A[L][L];
  for(I = 1; I <= K; I++) {
    VEC[I]=0.;
    for(J = 1; J <= K; J++) {
      VEC[I]=VEC[I]+R[I][J]*A[L][J];
    }/*J*/
    RL=RL-A[L][I]*VEC[I];
  }/*I*/
  DET=DET*RL;
  for(I = 1; I <= K; I++) {
    R[L][I]=-VEC[I]/RL;
    R[I][L]=R[L][I];
  }/*I*/
  for(I = 1; I <= K; I++) {
    for(J = I; J <= K; J++) {
      R[I][J]=R[I][J]-VEC[I]*R[L][J];
      R[J][I]=R[I][J];
    }/*J*/
  }/*I*/
  R[L][L]=1./RL;
}/*L*/
return(DET);
}/* SINV*/

/* SHELL-METZNER SORT */
#include "bc.h"
#define DLIM 101
#define NN 20
main() {
int X[DLIM];
/* GENERATE VECTOR */
for(I = 1; I <= N; I++) {
  X[I] = N-I+1;
}/*I*/
PRVEC(X,L);
SZSORT(X,L);
PRVEC(X,L);
}/* SORT*/

SZSORT(X,N)
int X,N;
{
int KT,TP,I,J, K = 1;
while(K < N) {
  K = 2*K;
}/*WH*/
K = K/2 - 1;
while(K >= 1) {
  KT=1;
  while(KT > 0) {
    J = K;
    KT = 0;
    for(I = 1; I <= N; I++) {
      J++;
      if(J <= N && X[I] > X[J]) {
        TP=X[I];X[I]=X[J];X[J]=TP;
        KT++;
      }/*IF*/
    }/*I*/
  }/*WH*/
  K = K/2;
}/*WH*/
}/* SZSORT*/

PRVEC(A,LL)
int A[], LL;
{
int I;
printf("\n");
for(I = 1; I <= LL; I++) {
  printf(" %d ", A[I]);
}/*I*/
printf("\n");
return;
}/* PRVEC*/

/* FIBONACCI NUMBERS */
#include "bc.h"
main() {
int  N;
printf("N = ");
IEOF = scanf("%d", &N);
printf("FIBON = %d\n", FIB(N));
}/* FIBNUM*/

int FIB(K)
int K;
{
if(K <= 2) {
  return(1);
} else {
  return(FIB(K-1) + FIB(K-2));
}/*IF*/
}/* FIB*/

/* ZERO OF FUNCTION BY NEWTON'S METHOD */
#include "bc.h"
main() {
int NMAX=20;
double TOL=1.0E-6, X0, X, NEWT();
X0 = 2;
X = NEWT(X0,TOL,NMAX);
printf("%f \n", X);
}/* NEWTON*/

double NEWT(X0,TOL,NMAX)
double X0,TOL; int NMAX;
{
double FN(), DFN(), fabs(), X, INC;
int I, N;
X = X0;
for(I = 1; I <= NMAX; I++) {
  INC = -FN(X)/DFN(X);
  X = X + INC;
  if(fabs(INC) < TOL) {
    return(X);
  }/*IF*/
}/*I*/
printf("NO CONVERGENCE");
exit(1);
}/* NEWT*/