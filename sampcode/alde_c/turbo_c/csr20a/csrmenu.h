/*
**  C S R M E N U . H
**
**  Menu structure definitions for use with the menu functions of 
**  the C Spot Run C Add-On Library.
**
**  Copyright 1986 Bob Pritchett
**
**  Created: 08/08/86   Last Updated: 08/16/86
**
*/

#define MAX_ENTRIES	15

typedef struct _mnu
 {
  char title[23];		/* Title of the menu */
  int type;			/* Style of the border */
  int border;			/* Window's border color */
  int normal;			/* Color for unhighlighted items */
  int bar;			/* Color for the highlight bar */
  int row;			/* Upper left window row */
  int col;			/* Upper left window column */
  int num;			/* Number of enteries */
  struct _ent
   {
    char text[25];		/* Item's name */
    int value;			/* Return value for item / -1 Static */
   } entry[MAX_ENTRIES];
 } *MENU;
