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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            