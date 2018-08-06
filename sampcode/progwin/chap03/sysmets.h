/*-----------------------------------------------
   SYSMETS.H -- System metrics display structure
  -----------------------------------------------*/

#define NUMLINES (sizeof sysmetrics / sizeof sysmetrics [0])

struct
     {
     int  nIndex ;
     char *szLabel ;
     char *szDesc ;
     }
     sysmetrics [] =
     {
     SM_CXSCREEN,      "SM_CXSCREEN",      "Screen width in pixels",
     SM_CYSCREEN,      "SM_CYSCREEN",      "Screen height in pixels",
     SM_CXVSCROLL,     "SM_CXVSCROLL",     "Vertical scroll arrow width",
     SM_CYHSCROLL,     "SM_CYHSCROLL",     "Horizontal scroll arrow height",
     SM_CYCAPTION,     "SM_CYCAPTION",     "Caption bar height",
     SM_CXBORDER,      "SM_CXBORDER",      "Border width",
     SM_CYBORDER,      "SM_CYBORDER",      "Border height",
     SM_CXDLGFRAME,    "SM_CXDLGFRAME",    "Dialog window frame width",
     SM_CYDLGFRAME,    "SM_CYDLGFRAME",    "Dialog window frame height",
     SM_CYVTHUMB,      "SM_CYVTHUMB",      "Vertical scroll thumb height",
     SM_CXHTHUMB,      "SM_CXHTHUMB",      "Horizontal scroll thumb width",
     SM_CXICON,        "SM_CXICON",        "Icon width",
     SM_CYICON,        "SM_CYICON",        "Icon height",
     SM_CXCURSOR,      "SM_CXCURSOR",      "Cursor width",
     SM_CYCURSOR,      "SM_CYCURSOR",      "Cursor height",
     SM_CYMENU,        "SM_CYMENU",        "Menu bar height",
     SM_CXFULLSCREEN,  "SM_CXFULLSCREEN",  "Full screen client window width",
     SM_CYFULLSCREEN,  "SM_CYFULLSCREEN",  "Full screen client window height",
     SM_CYKANJIWINDOW, "SM_CYKANJIWINDOW", "Kanji window height",
     SM_MOUSEPRESENT,  "SM_MOUSEPRESENT",  "Mouse present flag",
     SM_CYVSCROLL,     "SM_CYVSCROLL",     "Vertical scroll arrow height",
     SM_CXHSCROLL,     "SM_CXHSCROLL",     "Horizontal scroll arrow width",
     SM_DEBUG,         "SM_DEBUG",         "Debug version flag",
     SM_SWAPBUTTON,    "SM_SWAPBUTTON",    "Mouse buttons swapped flag",
     SM_RESERVED1,     "SM_RESERVED1",     "Reserved",
     SM_RESERVED2,     "SM_RESERVED2",     "Reserved",
     SM_RESERVED3,     "SM_RESERVED3",     "Reserved",
     SM_RESERVED4,     "SM_RESERVED4",     "Reserved",
     SM_CXMIN,         "SM_CXMIN",         "Minimum window width",
     SM_CYMIN,         "SM_CYMIN",         "Minimum window height",
     SM_CXSIZE,        "SM_CXSIZE",        "Minimize/Maximize icon width",
     SM_CYSIZE,        "SM_CYSIZE",        "Minimize/Maximize icon height",
     SM_CXFRAME,       "SM_CXFRAME",       "Window frame width",
     SM_CYFRAME,       "SM_CYFRAME",       "Window frame height",
     SM_CXMINTRACK,    "SM_CXMINTRACK",    "Minimum tracking width of window",
     SM_CYMINTRACK,    "SM_CYMINTRACK",    "Maximum tracking width of window",
     SM_CMETRICS,      "SM_CMETRICS",      "Number of system metrics"
     } ;
