/*
 *  linebuf.h
 */

typedef struct lb_st {
	struct lb_st *l_next;
	struct lb_st *l_prev;
	char *l_buf;
} LINEBUF;
