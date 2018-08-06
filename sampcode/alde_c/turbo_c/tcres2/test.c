#include <resident.h> 
 
int window_one_buff [ (37 + 2) * (4 + 2) ]; 
int run_once = 0, low_color = MONOCHROME, hi_color; 
int pass_buffer [ 35 * 2 + 1 ]; 
 
struct WINDOW window_one; 
 
void test_box () 
   { 
      int key; 
 
      if (run_once)  
         exch_window (&window_one); 
      else { 
         make_window (&window_one); 
         dis_str (11, 21, "Test of Memory Resident Library V2.01", low_color); 
         dis_str (12, 21, "Press the arrow & paging keys to move", low_color); 
         dis_str (13, 21, "Press ESC to uninstall the test Prog.", low_color); 
         dis_str (14, 25,     "Press any other key to exit.", low_color); 
         run_once = 1; 
      } 
      hidecur (); 
      while (!(key = getch ())) 
         key_window (getch (), &window_one);  
      keystrokes ("Test Passed!", pass_buffer);  
      exch_window (&window_one); 
      normalcur (); 
      if (key == 27) 
         drop_tsr (); 
   } 
 
main () 
   { 
      if (loaded ()) 
         printf ("The test program has already been loaded"); 
      else { 
         if (iscolor ()) 
            low_color = color (GREEN, BLACK); 
         hi_color = 10; 
         STACK_SIZE = 200; 
         window_one.row = 10; 
         window_one.col = 20; 
         window_one.hlen = 37; 
         window_one.vlen = 4; 
         window_one.color = hi_color; 
         window_one.buffer = window_one_buff; 
         window_one.vtype = DOUBLE_LINE; 
         window_one.htype = DOUBLE_LINE; 
         printf ("\n"); 
         printf ("Memory Resident Library V2.01 Test Program\n"); 
         printf ("Press Ctrl - Alt - T to call up.\n"); 
         go_resident (test_box, KEY_T, KEY_ALT, KEY_CTRL);  
      } 
   } 
 
