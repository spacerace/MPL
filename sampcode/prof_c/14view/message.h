/*
 *	message.h -- header for message line manager
 */

typedef struct msg_st {
	int m_row;
	int m_col;
	int m_wid;
	int m_pg;
	int m_flag;
	unsigned char m_attr;
} MESSAGE;
