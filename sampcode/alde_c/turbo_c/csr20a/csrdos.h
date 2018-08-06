/*
**  C Spot Run DOS Functions Header File
**
**  NOTE:  The functions that use this file need at least DOS 2.0
**         to properly function.
**
**  Alan Losoff's TREEDIR3.C contained the original DIRS structure.
**
**  Created: 04/04/86   Last Update: 04/15/86
**
*/

struct DIRS
 {
  char dos[21];		/* DOS Internal Use */
  char attribute;	/* File Attribute */
  struct
   {
    unsigned hour:5;
    unsigned minute:6;
    unsigned second:5;
   } time;
  struct
   {
    unsigned year:7;
    unsigned month:4;
    unsigned day:5;
   } date;
  long size;
  char name[13];
  char filler[85];
 };

#define READ_ONLY	0x01
#define HIDDEN	0x02
#define SYSTEM	0x04
#define VOLUME	0x08
#define SUB_DIR	0x10
#define ARCHIVE	0x20
