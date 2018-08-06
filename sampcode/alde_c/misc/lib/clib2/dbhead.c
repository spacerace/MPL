/*dbhead--contains dbase header definitions       */ 
/*---------- Macro Function Definitions ----------*/
#define	  db_tell(db_ptr)	db_ptr->curr_rec;
#define   db_recnum(db_ptr)	db_ptr->rec_num;
#define   db_reclen(db_ptr)     db_ptr->rec_len;
#define   db_varnum(db_ptr)     db_ptr->var_num;

/*-- Definition of the File Description Block -----*/
typedef   struct    {
                      char    name[11];
                      char    var_type;
                      char    var_len;
                      int     var_loc;
                      char    dec_pts;
                    } DB_DESC;  


/* Definition of the DBASE header record and internal working */
/*   variables                                                */

typedef   struct    {
                      char    version;                
			/* Version number.  Must be 02        */
             unsigned int     rec_num;                
			/* number of records                  */
                      char    day, month, year;       
			/* date of last modification dd/mm/yy */
             unsigned int     rec_len;                
			/* record length. sum(fields) + 1     */
                      DB_DESC field_desc[32];         
			/* field descriptors                  */
                      int     *file_ptr;              
			/* pointer to FCB from open command   */
                      char    var_num;                
			/* number of variables in the file    */
                      char    chng_ind;               
			/* boolean if file changed by db_write*/
                      int     curr_rec;               
			/* current record number              */
                    } DBASE_FILE;

/*end of dbase.h*/
