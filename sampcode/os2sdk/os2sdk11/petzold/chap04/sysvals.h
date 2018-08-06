/*----------------------------------------------
   SYSVALS.H -- System values display structure 
  ----------------------------------------------*/

#define NUMLINES (sizeof sysvals / sizeof sysvals [0])

struct
     {
     SHORT sIndex ;
     CHAR  *szIdentifier ;
     CHAR  *szDescription ;
     }
     sysvals [] =
     {
     SV_SWAPBUTTON,     "SV_SWAPBUTTON",     "Mouse buttons swapped flag",
     SV_DBLCLKTIME,     "SV_DBLCLKTIME",     "Mouse double click time in msec",
     SV_CXDBLCLK,       "SV_CXDBLCLK",       "Mouse double click area width",
     SV_CYDBLCLK,       "SV_CYDBLCLK",       "Mouse double click area height",
     SV_CXSIZEBORDER,   "SV_CXSIZEBORDER",   "Sizing border width",
     SV_CYSIZEBORDER,   "SV_CYSIZEBORDER",   "Sizing border height",
     SV_ALARM,          "SV_ALARM",          "Alarm enabled flag",
     SV_CURSORRATE,     "SV_CURSORRATE",     "Cursor blink rate",
     SV_FIRSTSCROLLRATE,"SV_FIRSTSCROLLRATE","Scroll bar time until repeats",
     SV_SCROLLRATE,     "SV_SCROLLRATE",     "Scroll bar scroll rate",
     SV_NUMBEREDLISTS,  "SV_NUMBEREDLISTS",  "Flag for numbering of lists",
     SV_WARNINGFREQ,    "SV_WARNINGFREQ",    "Alarm frequency for warning",
     SV_NOTEFREQ,       "SV_NOTEFREQ",       "Alarm frequency for note",
     SV_ERRORFREQ,      "SV_ERRORFREQ",      "Alarm frequency for error",
     SV_WARNINGDURATION,"SV_WARNINGDURATION","Alarm duration for warning",
     SV_NOTEDURATION,   "SV_NOTEDURATION",   "Alarm duration for note",
     SV_ERRORDURATION,  "SV_ERRORDURATION",  "Alarm duration for error",
     SV_CXSCREEN,       "SV_CXSCREEN",       "Screen width in pixels",
     SV_CYSCREEN,       "SV_CYSCREEN",       "Screen height in pixels",
     SV_CXVSCROLL,      "SV_CXVSCROLL",      "Vertical scroll bar width",
     SV_CYHSCROLL,      "SV_CYHSCROLL",      "Horizontal scroll bar height",
     SV_CYVSCROLLARROW, "SV_CYVSCROLLARROW", "Vertical scroll arrow height",
     SV_CXHSCROLLARROW, "SV_CXHSCROLLARROW", "Horizontal scroll arrow width",
     SV_CXBORDER,       "SV_CXBORDER",       "Border width",
     SV_CYBORDER,       "SV_CYBORDER",       "Border height",
     SV_CXDLGFRAME,     "SV_CXDLGFRAME",     "Dialog window frame width",
     SV_CYDLGFRAME,     "SV_CYDLGFRAME",     "Dialog window frame height",
     SV_CYTITLEBAR,     "SV_CYTITLEBAR",     "Title bar height",
     SV_CYVSLIDER,      "SV_CYVSLIDER",      "Vertical scroll slider height",
     SV_CXHSLIDER,      "SV_CXHSLIDER",      "Horizontal scroll slider width",
     SV_CXMINMAXBUTTON, "SV_CXMINMAXBUTTON", "Minimize/Maximize button width",
     SV_CYMINMAXBUTTON, "SV_CYMINMAXBUTTON", "Minimize/Maximize button height",
     SV_CYMENU,         "SV_CYMENU",         "Menu bar height",
     SV_CXFULLSCREEN,   "SV_CXFULLSCREEN",   "Full screen client window width",
     SV_CYFULLSCREEN,   "SV_CYFULLSCREEN",   "Full screen client window height",
     SV_CXICON,         "SV_CXICON",         "Icon width",
     SV_CYICON,         "SV_CYICON",         "Icon height",
     SV_CXPOINTER,      "SV_CXPOINTER",      "Pointer width",
     SV_CYPOINTER,      "SV_CYPOINTER",      "Pointer height",
     SV_DEBUG,          "SV_DEBUG",          "Debug version flag",
     SV_CMOUSEBUTTONS,  "SV_CMOUSEBUTTONS",  "Number of mouse buttons",
     SV_POINTERLEVEL,   "SV_POINTERLEVEL",   "Pointer display count",
     SV_CURSORLEVEL,    "SV_CURSORLEVEL",    "Cursor display count",
     SV_TRACKRECTLEVEL, "SV_TRACKRECTLEVEL", "Tracking rectangle display count",
     SV_CTIMERS,        "SV_CTIMERS",        "Number of available timers",
     SV_MOUSEPRESENT,   "SV_MOUSEPRESENT",   "Mouse present flag",
     SV_CXBYTEALIGN,    "SV_CXBYTEALIGN",    "Horizontal pixel alignment value",
     SV_CYBYTEALIGN,    "SV_CYBYTEALIGN",    "Vertical pixel alignment value"
     } ;
