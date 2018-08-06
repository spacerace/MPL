From hplabs!hao!seismo!rlgvax!cvl!umcp-cs!fred Wed Dec 31 16:00:00 1969
Relay-Version: version B 2.10.1     9/27/83; site hplabsb.UUCP
Posting-Version: version B 2.10 5/3/83; site umcp-cs.UUCP
Path: hplabsb!hplabs!hao!seismo!rlgvax!cvl!umcp-cs!fred
From: fred@umcp-cs.UUCP
Newsgroups: net.sources
Subject: matrix manipulation subroutine library in C
Message-ID: <1184@umcp-cs.UUCP>
Date: Wed, 27-Jul-83 14:35:25 PST
Article-I.D.: umcp-cs.1184
Posted: Wed Jul 27 14:35:25 1983
Date-Received: Thu, 28-Jul-83 00:29:01 PDT
Organization: Univ. of Maryland, Computer Science Dept.
Lines: 472

These might be of general interest. Sorry I don't have a makefile
for it, but there are no complex dependency problems.

: Run this shell script with "sh" not "csh"
PATH=:/bin:/usr/bin:/usr/ucb
export PATH
all=FALSE
if [ $1x = -ax ]; then
	all=TRUE
fi
/bin/echo 'Extracting matrix.3l'
sed 's/^X//' <<'//go.sysin dd *' >matrix.3l
X.TH MATRIX 3L 12-Oct-1982
X.SH NAME
m_cofactor,  m_copy, m_determ, m_invert, m_multiply, m_solve,
m_transpose \- matrix manipulation subroutines
X.SH SYNOPSIS
X.B #include <local/mat.h>
X.br
X.B cc "*.c" -lmatrix
X.PP
X.B double m_cofactor(m, i, j);
X.br
X.B struct matrix *m;
X.br
X.B int i, j;
X.PP
X.B struct matrix *m_copy(m);
X.br
X.B struct matrix *m;
X.br
X.PP
X.B double m_determ(m);
X.br
X.B struct matrix *m;
X.PP
X.B struct matrix *m_invert(m);
X.br
X.B struct matrix *m;
X.PP
X.B struct matrix *m_multiply(m1, m2);
X.br
X.B struct matrix *m1;
X.br
X.B struct matrix *m2;
X.PP
X.B struct matrix *m_solve(a, b);
X.br
X.B struct matrix *a;
X.br
X.B struct matrix *b;
X.PP
X.B struct matrix *m_transpose(m);
X.br
X.B struct matrix *m;
X.SH DESCRIPTION
This set of subroutines does several useful operations on matrices which are
stored in a standard format (defined in /usr/include/local/matrix.h).
Space for matrices is allocated and freed by the subroutines
X.I malloc
and
X.I free.
Matrices should be initialized by the macro
X.I m_create,
and can be disposed of by passing a pointer to the matrix to
X.I free.
X.PP
X.I m_cofactor\c
(m, i, j) returns the cofactor, as type
X.I double,
of element (i, j) of the matrix given by its first argument.
That is: the determinant of the matrix obtained by deleting row
X.I i
and column
X.I j
from the matrix
X.I m,
multiplied by
X.br
(-1)**(i+j).
X.PP
X.I m_copy\c
(m) returns a duplicate copy of the matrix
X.I m.
X.PP
X.I m_determ\c
(m) returns the determinant of the matrix m as type
X.I double.
X.PP
X.I m_invert\c
(m) returns a matrix (if it exists) which is the inverse of the matrix
X.I m.
X.PP
X.I m_multiply\c
(m1, m2) returns a matrix which is the result of multiplying matrix
X.I m1
by matrix
X.I m2
by the conventional definition of matrix multiplication. The number of
columns in
X.I m1
must be the same as the number of rows in
X.I m2.
X.PP
X.I m_solve\c
(a, b) returns the matrix which results from computing: (a'\ a)**(-1)\ a'\ b.
This is useful for solving a set of linear equations expressed as matrices:
a\ x\ =\ b.
X.PP
X.I m_transpose\c
(m) returns the matrix which is the transpose of
X.I m.
X.PP
X.I m_create\c
(m, r, c) (which is a macro, not a function) allocates space for,
and initializes the matrix
X.I
m
to have
X.I r
rows and
X.I c
columns.
X.PP
X.I m_v\c
(m, r, c) is a macro used to access element
X.I (r, c)
of the matrix
X.I m.
The elements are always of type double.
X.SH AUTHOR
Fred Blonder <fred@umcp-cs>
X.SH "SEE ALSO"
malloc(3), free(3), printf(3)
X.SH DIAGNOSTICS
Most routines return the constant
X.I M_NULL
to report an error. Some also write messages to the standard output via
X.I printf.
X.SH FILES
X/usr/include/local/matrix.h
X.br
X/usr/lib/libmatrix.a
X.SH BUGS
Error reporting via
X.I printf
should be an option.
//go.sysin dd *
made=TRUE
if [ $made = TRUE ]; then
	/bin/chmod 644 matrix.3l
	/bin/echo -n '	'; /bin/ls -ld matrix.3l
fi
/bin/echo 'Extracting mat.h'
sed 's/^X//' <<'//go.sysin dd *' >mat.h
struct matrix {
	int m_rows, m_cols;
	};

struct m_ {
	int m_rows, m_cols;
	double m_value[1];
	};

double m_cofactor(), m_determinant();
struct matrix *m_copy(), *m_invert(), *m_transpose(), *m_multiply(), *m_solve();

#define	m_v(m, r, c)	((struct m_ *)m)->m_value[r * (m->m_cols) + c]
#define	M_NULL	((struct matrix *)0)

#define	m_create(m, r, c) {\
			if (((int)(m = (struct matrix *)malloc(sizeof(struct matrix) + (sizeof(double) * r * c)))) == 0) {\
				printf("Allocation error: %s\n", __FILE__);\
				exit(1);\
				}\
			m->m_rows = r;\
			m->m_cols = c;\
			}
//go.sysin dd *
made=TRUE
if [ $made = TRUE ]; then
	/bin/chmod 644 mat.h
	/bin/echo -n '	'; /bin/ls -ld mat.h
fi
/bin/echo 'Extracting m_cofactor.c'
sed 's/^X//' <<'//go.sysin dd *' >m_cofactor.c
static char *sccsid = "@(#)m_cofactor.c	4/5/82 (U of Maryland, FLB)";

#include "mat.h"

double
m_cofactor(mat, i, j)
register struct matrix *mat;
register int i, j;
{
register struct matrix *result;
register int row, col, o_row = 0, o_col = 0;
double det;

m_create(result, mat->m_rows - 1, mat->m_cols - 1);

for (row = 0; row < mat->m_rows; row++)
	for (col = 0; col < mat->m_cols; col++)
		if (row != i && col != j)
			m_v(result, o_row++, o_col++) = m_v(mat, row, col);

det = m_determinant(result);
free(result);

return(((i + j) & 01)? -det: det);
}
//go.sysin dd *
made=TRUE
if [ $made = TRUE ]; then
	/bin/chmod 644 m_cofactor.c
	/bin/echo -n '	'; /bin/ls -ld m_cofactor.c
fi
/bin/echo 'Extracting m_copy.c'
sed 's/^X//' <<'//go.sysin dd *' >m_copy.c
static char *sccsid = "@(#)m_copy.c	4/5/82 (U of Maryland, FLB)";

#include "mat.h"

struct matrix *
m_copy(mat)
register struct matrix *mat;
{
register struct matrix *result;
register int row, col;

m_create(result, mat->m_rows, mat->m_cols);

for (row = 0; row < mat->m_rows; row++)
	for (col = 0; col < mat->m_cols; col++)
		m_v(result, row, col) = m_v(mat, row, col);

return(result);
}
//go.sysin dd *
made=TRUE
if [ $made = TRUE ]; then
	/bin/chmod 644 m_copy.c
	/bin/echo -n '	'; /bin/ls -ld m_copy.c
fi
/bin/echo 'Extracting m_determ.c'
sed 's/^X//' <<'//go.sysin dd *' >m_determ.c
static char *sccsid = "@(#)m_determinant.c	4/5/82 (U of Maryland, FLB)";

#include "mat.h"

double
m_determinant(mat)
register struct matrix *mat;
{
register int col;
double det = 0.0;

if (mat->m_rows == 1)
	return(m_v(mat, 0, 0));

for (col = 0; col < mat->m_cols; col++)
	det += 	m_v(mat, 0, col) * m_cofactor(mat, 0, col);

return(det);
}
//go.sysin dd *
made=TRUE
if [ $made = TRUE ]; then
	/bin/chmod 644 m_determ.c
	/bin/echo -n '	'; /bin/ls -ld m_determ.c
fi
/bin/echo 'Extracting m_dump.c'
sed 's/^X//' <<'//go.sysin dd *' >m_dump.c
static char *sccsid = "@(#)m_dump.c	4/6/82 (U of Maryland, FLB)";

#include "mat.h"

struct matrix *
m_dump(mat)
register struct matrix *mat;
{
register int row, col;

printf("%d X %d\n", mat->m_rows, mat->m_cols);

for (row = 0; row < mat->m_rows; row++) {
	for (col = 0; col < mat->m_cols; col++)
		printf("%f, ", m_v(mat, row, col));
	putchar('\n');
	}
}
//go.sysin dd *
made=TRUE
if [ $made = TRUE ]; then
	/bin/chmod 644 m_dump.c
	/bin/echo -n '	'; /bin/ls -ld m_dump.c
fi
/bin/echo 'Extracting m_invert.c'
sed 's/^X//' <<'//go.sysin dd *' >m_invert.c
static char *sccsid = "@(#)m_invert.c	4/7/82 (U of Maryland, FLB)";

#include "mat.h"

struct matrix *
m_invert(mat)
register struct matrix *mat;
{
register struct matrix *result;
register int row, col;
double det;

if((det = m_determinant(mat)) == 0.0) {
	printf("m_invert: singular matrix\n");
	return(M_NULL);
	}

m_create(result, mat->m_cols, mat->m_rows);

for (row = 0; row < mat->m_rows; row++)
	for (col = 0; col < mat->m_cols; col++)
		m_v(result, col, row) = m_cofactor(mat, row, col) / det;

return(result);
}
//go.sysin dd *
made=TRUE
if [ $made = TRUE ]; then
	/bin/chmod 644 m_invert.c
	/bin/echo -n '	'; /bin/ls -ld m_invert.c
fi
/bin/echo 'Extracting m_multiply.c'
sed 's/^X//' <<'//go.sysin dd *' >m_multiply.c
static char *sccsid = "@(#)m_multiply.c	4/6/82 (U of Maryland, FLB)";

#include "mat.h"

struct matrix *
m_multiply(mat1, mat2)
register struct matrix *mat1, *mat2;
{
register struct matrix *result;
register int row, col, ix;
double sum;

if (mat1->m_cols != mat2->m_rows) {
	printf("m_multiply: matrices not compatible.\n");
	return(M_NULL);
	}

m_create(result, mat1->m_rows, mat2->m_cols);

for (row = 0; row < mat1->m_rows; row++)
	for (col = 0; col < mat2->m_cols; col++) {
		sum = 0.0;
		for (ix = 0; ix < mat1->m_cols; ix++)
			sum += m_v(mat1, row, ix) * m_v(mat2, ix, col);
		m_v(result, row, col) = sum;
		}

return(result);
}
//go.sysin dd *
made=TRUE
if [ $made = TRUE ]; then
	/bin/chmod 644 m_multiply.c
	/bin/echo -n '	'; /bin/ls -ld m_multiply.c
fi
/bin/echo 'Extracting m_read.c'
sed 's/^X//' <<'//go.sysin dd *' >m_read.c
static char *sccsid = "@(#)m_read.c	4/6/82 (U of Maryland, FLB)";

#include <stdio.h>
#include "mat.h"

struct matrix *
m_read()
{
register struct matrix *result;
register int row, col;
int rows, cols;

scanf("%d%d", &rows, &cols);
m_create(result, rows, cols);

for (row = 0; row < rows; row++)
	for (col = 0; col < cols; col++)
		scanf("%lf", &m_v(result, row, col));

return(result);
}
//go.sysin dd *
made=TRUE
if [ $made = TRUE ]; then
	/bin/chmod 644 m_read.c
	/bin/echo -n '	'; /bin/ls -ld m_read.c
fi
/bin/echo 'Extracting m_solve.c'
sed 's/^X//' <<'//go.sysin dd *' >m_solve.c
static char *sccsid = "@(#)m_solve.c	4/16/82 (U of Maryland, FLB)";

#include "mat.h"

struct matrix *
m_solve(a, b)
register struct matrix *a, *b;
{
register struct matrix *a_trans, *t, *t_inv, *t2, *x;

if ((a->m_rows) < (a->m_cols)) {
	printf("m_solve: too few equations\n");
	return(M_NULL);
	}

if ((a->m_rows) != (b->m_rows)) {
	printf("m_solve: arguments don't match: %d, %d.\n", a->m_rows, b->m_rows);
	return(M_NULL);
	}

if (b->m_cols != 1) {
	printf("m_solve: arg2 must have 1 column.\n");
	return(M_NULL);
	}

a_trans = m_transpose(a);		/* A' */
t = m_multiply(a_trans, a);		/* A' A */
t_inv = m_invert(t);			/* (A' A)-1 */
free(t);
if (t_inv == M_NULL) {
	printf("m_solve: no solution\n");
	return(M_NULL);
	}
t2 = m_multiply(t_inv, a_trans);	/* (A' A)-1 A' */
free(t_inv);
free(a_trans);
x = m_multiply(t2, b);			/* (A' A)-1 A' B */
free(t2);

return(x);
}
//go.sysin dd *
made=TRUE
if [ $made = TRUE ]; then
	/bin/chmod 644 m_solve.c
	/bin/echo -n '	'; /bin/ls -ld m_solve.c
fi
/bin/echo 'Extracting m_transpose.c'
sed 's/^X//' <<'//go.sysin dd *' >m_transpose.c
static char *sccsid = "@(#)m_transpose.c	4/5/82 (U of Maryland, FLB)";

#include "mat.h"

struct matrix *
m_transpose(mat)
register struct matrix *mat;
{
register struct matrix *result;
register int row, col;

m_create(result, mat->m_cols, mat->m_rows);

for (row = 0; row < mat->m_rows; row++)
	for (col = 0; col < mat->m_cols; col++)
		m_v(result, col, row) = m_v(mat, row, col);

return(result);
}
//go.sysin dd *
made=TRUE
if [ $made = TRUE ]; then
	/bin/chmod 644 m_transpose.c
	/bin/echo -n '	'; /bin/ls -ld m_transpose.c
fi


                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               