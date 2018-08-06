/*

Microsoft Systems Journal
Volume 3; Issue 3; May, 1988

Code Listings For:

	DLL_CHAT; DLL_LIB; CHATMEM
	pp. 27-48


Author(s): Ross M. Greenberg
Title:     Design Concepts and Considerations in Building an OS/2
           Dynamic-Link Library

*/

/* Header file for CHAT */

struct gdtinfoarea{
        unsigned long   time;
        unsigned long   milliseconds;
        unsigned char   hours;
        unsigned char   minutes;
        unsigned char   seconds;
        unsigned char   hundreths;
        unsigned        timezone;
        unsigned        timer_interval;
        unsigned char   day;
        unsigned char   month;
        unsigned        year;
        unsigned char   day_of_week;
        unsigned char   major_version;
        unsigned char   minor_version;
        unsigned char   revision_number;
        unsigned char   current_screen_group;
        unsigned char   max_num_of_screengrps;  
        unsigned char   huge_selector_shift_count;
        unsigned char   protect_mode_indicator;
	unsigned	foreground_process_id;
        unsigned char   dynamic_variation_flag;
        unsigned char   maxwait;
        unsigned        minimum_timeslice;
        unsigned        maximum_timeslice;
	unsigned	boot_drive;
	unsigned char	reserved[32];
 };


 struct ldtinfoarea{
        unsigned        current_process_pid;
        unsigned        parent_pid;
        unsigned        priority_of_current_thread;
        unsigned        thread_id_of_current_thread;
        unsigned        screen_group;
        unsigned        subscreen_group;
        unsigned        current_process_is_in_fg;
 };
