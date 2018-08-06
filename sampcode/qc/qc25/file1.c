/* FILE1.C: Visibility in multiple source files.
*/

int chico = 20, harpo = 30;
extern void yonder( void );

main()
{
   yonder();
}
