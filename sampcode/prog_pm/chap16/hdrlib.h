/*--------------------------------------------------
   HDRLIB.H -- "Handy Drawing Routines" Header File
  --------------------------------------------------*/

SHORT APIENTRY  HdrPuts    (HPS hps, PPOINTL pptl, PCHAR szText) ;
SHORT cdecl FAR HdrPrintf  (HPS hps, PPOINTL pptl, PCHAR szFormat, ...) ;
LONG  APIENTRY  HdrEllipse (HPS hps, LONG lOption, PPOINTL pptl) ;
