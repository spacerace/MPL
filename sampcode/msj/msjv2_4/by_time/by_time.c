/*

Microsoft Systems Journal
Volume 2; Issue 4; September, 1987

Code Listings For:

	BY_TIME
	pp. 35-45

Author(s): Steve Schustack
Title:     Dynamic Allocation Techniques for Memory Management in C Programs



Figure 15
=========

*/

/****************************************************************
 * BY_TIME.C: Filter sorts DIR output in linked list, most recent 
 * first. This is the main function of the program.
 */

#include <stdio.h>                      /* For BUFSIZ symbol   */
#include "by_time.h"                    /* Linked list struct  */
main()
        {
        struct s_dir_node head;         /* First node in list  */
        char in_buf[BUFSIZ];            /* Input buffer for    */
                                        /*   DIR output        */
        bgn_list(&head);                /* Initialize list     */
        while (get_dir(in_buf))         /* Input DIR info for  */
                                        /*   next file         */
                sav_dir(in_buf, &head); /* Save file info in   */
                                        /*  sorted linked list */
        sho_list(&head);                /* Show sorted list    */
        }

/*

Figure 16
=========

*/

/****************************************************************
 * BGN_LIST.C: Initialize head of list to dummy highest value  
 */
#include <stdio.h>              /* For NULL (zero) pointer     */
#include "by_time.h"            /* For linked list struct      */
 
bgn_list(p_head)
struct s_dir_node *p_head;      /* Pointer to head of list     */
        {
        /* Date in head is greater than DIR date for any file  */
        strcpy(p_head->dir_line, 
                "ZZZZZZZZ ZZZ    99999  99-99-99  99:99p");
        p_head->next = NULL;    /* No next node - empty list   */
        }

/*

Figure 17
=========

*/

/****************************************************************
 * GET_DIR.C:  Input DIR info for next fil
e from stdin
 */
#include <stdio.h>              /* For NULL (zero) pointer     */

int get_dir(buf)        
char *buf;              /* Buffer for read and pass line back  */
        {
        char *rtn;                      /* save gets() return  */

        /* Loop:  Input lines until no more input or got line  */
        /*   starting with an uppercase letter (has file data) */
        while ((rtn = gets(buf)) &&     /* Input a DIR line    */
                (buf[0] < 'A' || buf[0] > 'Z'))   /* For file? */
                ;
        return (rtn != NULL);  /* Return 1 if got data, else 0 */
        }        

/*

Figure 18
=========

*/

/****************************************************************
 * SAV_DIR.C: Allocate new node in list, save DIR info in it
 */
#include <stdio.h>              /* For NULL (zero) pointer     */
#include "by_time.h"            /* For linked list struct      */

sav_dir(buf, p_head)
char *buf;                      /* Line of DIR output to save  */
struct s_dir_node *p_head;      /* Pointer to head of list     */
        {
        struct s_dir_node *p_next;      /* Pointer to next     */
                                        /*   node in list      */
        struct s_dir_node *old_p_next;  /* Pointer to previous */
                /*   next node, parent of current next node    */
        struct s_dir_node *p_new;       /* Pointer to new node */

        /* Loop: for each node in list until end of list or    */
        /*   insert point that will keep list sorted is found  */
        for (p_next = p_head->next, old_p_next = p_head;
                p_next && time_b4(buf, p_next);         
                old_p_next = p_next, p_next = p_next->next)
                ;
        /* Dynamically allocate memory for new node - DIR output 
         *   line. Note use of the cast (struct s_dir_node *) 
         *   operator in the assignment to avoid this message: 
         *         warning 47: '=' : different levels of 

         *                 indirection
         */
        p_new = (struct s_dir_node *) 
                malloc(sizeof (struct s_dir_node));
        if (p_new == NULL)      /* malloc() failed, out of RAM */
                {
                puts("Out of memory!!!");
                return;
                }
        strcpy(p_new->dir_line, buf);   /* Save DIR line in    */
                                        /*  newly alloc'd node */
        p_new->next = old_p_next->next; /* New node points to  */
                                        /*   rest of list      */
        old_p_next->next = p_new;       /* Insert new node in  */
                                        /*   list              */
        }

/*

Figure 19
=========

*/

/****************************************************************
 * TIME_B4.C: Return 1 if date and time in buf is before date 
 *   and time in node p_next points to, otherwise return 0.
 */
#include "by_time.h"            /* For linked list struct      */
int time_b4(buf, p_next)
char *buf;                      /* Line of DIR output to find  */ 
                                /*   insert point in list for  */
struct s_dir_node *p_next;      /* Pointer to node in list to  */
        {                       /*   compare time in buf with  */
        int rtn;                /* Return value from strncmp() */

        /* compare year, month, day, am/pm, hour, and minute   */

        if (rtn = strncmp(&buf[29], &(p_next->dir_line)[29], 2))
                 return (rtn < 0);      /* Years differ        */
        if (rtn = strncmp(&buf[23], &(p_next->dir_line)[23], 2)) 
                 return (rtn < 0);      /* Months differ       */
        if (rtn = strncmp(&buf[26], &(p_next->dir_line)[26], 2)) 
                 return (rtn < 0);      /* Days differ         */
        if (buf[38] != (p_next->dir_line)[38])  /* am/pm's     */ 
                return (buf[38] == 'a');        /* differ      */
        if (rtn = strncmp(&buf[33], &(p_next->dir_line)[33], 2)) 
                 return (rtn < 0);      /* Hours differ        */
        if (rtn = strncmp(&buf[36], &(p_next->dir_line)[36], 2)) 
                 return (rtn < 0);      /*  Minutes differ     */
        return (0);             /* Dates and times are equal   */
        }

*/

Figure 20
=========

*/

/****************************************************************
 * SHO_LIST.C: Show sorted linked list - output it to stdout   
 */
#include <stdio.h>              /* For NULL (zero) pointer     */
#include "by_time.h"            /* Linked list struct          */

sho_list(p_head)
struct s_dir_node *p_head;      /* Pointer to head of list     */
        {
        struct s_dir_node *p_next;      /* Pointer to next     */
                                        /*   node in list      */

        for (p_next = p_head->next;     /* Start at first node */
                p_next != NULL;         /* Still more list?    */
                p_next = p_next->next)  /* Move down a node    */

                puts(p_next->dir_line); /* Output a DIR line   */
        }