/*

Figure 14
=========

*/

/****************************************************************
 * BY_TIME.H:  Header file for BY_TIME program, define linked  
 *   list structure as next node pointer and line of DIR text  
 */
#define DIR_LINE_LEN 39         /* DIR output line length      */
struct s_dir_node               /* Linked list node structure  */
        {
        struct s_dir_node *next;         /* Next node pointer  */
                                         /*   or NULL (0)      */
        char dir_line[DIR_LINE_LEN + 1]; /* DIR output line    */
        };
