#include "stdio.h"

/* This function displays the contents of a specific spreadsheet cell */
main()
{

	char filename[12];
	
	int open_BIFF(), find_BIFF(), close_BIFF();
	int type, row, col;
	int handler(int, char *, int);

		
	printf("Enter filename\n");
	scanf("%s", filename);
	printf("Enter type\n");
	scanf("%d", &type);
	printf("Enter row\n");
	scanf("%d", &row);
	printf("Enter col\n");
	scanf("%d", &col);
	
	open_BIFF(filename);
	find_BIFF(type, row, col, handler);
}

int handler(int type, char *data, int length)
{
	int row, column, i;
	unsigned int value;
	double val;
	char *label;
	
	int get_blank(char *, int *, int *);
	int get_integer(char *, int *, int *, unsigned int *);
	int get_number(char *, int *, int *, double *);
	int get_label(char *, int *, int *,  char **);
	int get_bool(char *, int *, int *, int *);


	switch (type) {
	    case 1:
		get_blank(data, &row, &column);
		printf("In row %d column %d is a blank\n", row, column); 
		break;
	    case 2:
		get_integer(data, &row, &column, &value);
		printf("In row %d column %d is %d\n", row, column, value); 
		break;
	    case 4:
		get_number(data, &row, &column, &val);
		printf("In row %d column %d is %f\n", row, column, val); 
		break;
	    case 8:
		get_label(data, &row, &column, &label);
		printf("In row %d column %d is %s\n", row, column, label); 
		break;
	    case 16:
		get_bool(data, &row, &column, &value);
		printf("In row %d column %d is %d\n", row, column, value); 
		break;
	}
	
}

