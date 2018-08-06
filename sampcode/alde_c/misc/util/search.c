/* 	int search(int (*dir_procedure)(),int (*file_procedure)(),char *mask);
**
**				this procedure will visit every subdirectory of the
**				directory where this procedure was called.  each time
**				a directory is entered or exited, the procedure pointer
**				dir_procedure is called.  each directory is searched for files
**				matching	the mask argument.  everytime a file is found matching
**				the mask, the file_procedure pointer is called.
**
**				int (*dir_procedure)(int pass,
**											char *dir[MAXDRIVE+MAXPATH+MAXFILE])
**	
**				the dir_procedure is called each time a directory is
**				entered (pass == 1) and after a directory is exited
**				(pass == 2).  dir_procedure is passed two arguments:
**	
**					ARGUMENTS:
**					pass = 1 on first entry to directory, pass = 2 on exit.
**					dir point to directory string.  on pass one it contains the
**					current working directory.  on pass two it contains the
**					directory of the exited string;
**	
**					RETURN:
**					returns of anything other than 0 terminates search and the
** 				return value is returned by search.
**
**				int (*file_procedure)(struct *ffblk);
**
**				the file_procedure pointer argument is executed each time
**				a new file name is encountered in the directory tree.  ALL
**				files trigger this call (HIDDEN and SYSTEM included).
**				The ffblk structure pointer is passed to the file_procedure
**				so file name and attributes may be tested.
**
**				calls to argument pointers are checked for NULL pointers.
*/

#include <stdio.h>
#include <alloc.h>
#include <dir.h>
#include <dos.h>

int search(int (*dir_procedure)(),int (*file_procedure)(),char *mask)
{
	struct ffblk *item;	/* see page 88 of turboc reference guide            */
	int errcode=0;			/* error code returned by directory find routines   */
	int returncode=0;		/* return code from called procedures               */
	char *dir;				/* current directory string                         */

	/* item and dir are allocated storage with malloc to minimize            */
	/* stack ussage durring recusive calls                                   */
	if (((item=(struct ffblk *)malloc(sizeof(struct ffblk))) == NULL) ||
		((dir=malloc(MAXDRIVE+MAXPATH+MAXFILE))==NULL))
		perror("Search Procedure");

	getcwd(dir,MAXDRIVE+MAXPATH);

	if(dir_procedure!=NULL)		/* only call procedure if not null            */
		if((returncode=(*dir_procedure)(1,dir))!=0)
			return(returncode);

	/*                                                                       */
	/* file loop                                                             */
	/*                                                                       */

	errcode=findfirst(mask,item,FA_SYSTEM+FA_HIDDEN);
	while(errcode==0  && returncode==0)
		{
		if (!(item->ff_attrib & FA_DIREC))
			if(file_procedure!=NULL)
				returncode=(*file_procedure)(item);
		if (!returncode)
			{
			errcode=findnext(item);
			}
		}

	/*                                                                       */
	/* directory loop                                                        */
	/*                                                                       */

	if (!returncode)
		errcode=findfirst("*.*",item,FA_DIREC+FA_SYSTEM+FA_HIDDEN);
	while(errcode==0 && returncode==0)
		{
		if((item->ff_attrib & FA_DIREC) && (item->ff_name[0]!='.'))
			{
	  		chdir(item->ff_name);				 /* drop down to subdir	          */

			/* recursive call */
			returncode=search(dir_procedure,file_procedure,mask);

			getcwd(dir,MAXDRIVE+MAXPATH);

  			chdir("..");		 					/* up to parent directory 	       */

			if(dir_procedure!=NULL)				/* pass two call to dir_procedure */
				if((returncode=(*dir_procedure)(2,dir))!=0)
					return(returncode);

  			}

		if (!returncode)
			errcode=findnext(item);
		}
	free(item);
	free(dir);
	return(returncode);
}

/* 
**		to see how this procedure works, add line: 	#define DEBUG
**		at top of file and compile/link with:			tcc search
*/ 

#ifdef DEBUG

int dir_print(int *pass, char *dir)
{
	printf("Directory: %s, pass %1d\n",dir,pass);
	return(0);
}

int file_print(struct ffblk *item)
{
	printf("File: %s\n",item->ff_name);
	return(0);
}

main(argc,argv)
int argc;
char **argv;
{
	if (argc==2)
		search(dir_print,file_print,argv[1]);
	else
	 	search(dir_print,file_print,"*.*");
}
#endif
