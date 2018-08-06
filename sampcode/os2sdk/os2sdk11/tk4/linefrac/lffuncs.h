/*
    Headers of LineFrac Functions
    Created by Microsoft Corporation, 1989
*/

#ifdef INCL_LFMAIN
/* linefrac.c */
VOID cdecl	 main (VOID);
ULONG CALLBACK	 LineFracWndProc    (HWND, USHORT, MPARAM, MPARAM);
VOID		 LfClose	    (HWND);
VOID		 LfEraseBackground  (HWND, HPS, PRECTL, PRECTL);
VOID		 LfPaint	    (HPS, PRECTL);
VOID		 LfShowSelectRc     (HPS, PRECTL);
VOID		 LfChar 	    (HWND, char);
VOID		 LfMouseMove	    (VOID);
VOID		 LfButtonUp	    (USHORT);
VOID		 LfButtonDown	    (HWND, MPARAM);
VOID		 LfStartRedrawTimer (VOID);
VOID		 LfStopRedrawTimer  (VOID);
#endif

#ifdef INCL_LFINIT
/* lfinit.c */
BOOL  FAR PASCAL LfInitApp (VOID);
#endif

#ifdef INCL_LFTHREAD
/* lfthread.c */
PTHR		 LfStartThread	    (USHORT);
VOID		 LfPreInitThread    (PTHR);
VOID		 LfPostInitThread   (PTHR);
VOID		 LfKillThread	    (PTHR);
VOID		 LfBringThreadToTop (PTHR);
BOOL		 LfIsThreadTop	    (PTHR);
VOID		 LfUpdateThreadMenu (VOID);
#endif

#ifdef INCL_LFPS
/* lfps.c */
BOOL		 LfOpenDC	(PTHR);
VOID		 LfCloseDC	(PTHR);
BOOL		 LfCreateBitmap (PTHR);
VOID		 LfDeleteBitmap (PTHR);
BOOL		 LfCreatePS	(PTHR);
VOID		 LfDeletePS	(PTHR);
BOOL		 LfResizePS	(PTHR);
#endif

#ifdef INCL_LFCMD
/* lfcmd.c */
VOID		 LfCommand	   (HWND, USHORT);
VOID		 LfFileMenu	   (HWND, USHORT);
VOID		 LfEditMenu	   (HWND, USHORT);
VOID		 LfThreadMenu	   (HWND, USHORT);
BOOL		 LfControlMenu	   (HWND, USHORT);
BOOL		 LfFractalMenu	   (HWND, USHORT);
BOOL		 LfPrimitiveMenu   (HWND, USHORT);
BOOL		 LfAttributesMenu  (HWND, USHORT);
VOID		 LfEnvironmentMenu (HWND, USHORT);
VOID		 LfSelectDimension (HWND, POINTS);
VOID		 LfSquareBitmap    (HWND);
VOID		 LfResizeBitmapToWindow (VOID);
VOID		 LfSelect	   (HWND, POINTS);
VOID		 LfSelectAll	   (HWND);
VOID		 LfCut		   (HWND);
BOOL		 LfCopy 	   (HWND);
VOID		 LfPaste	   (HWND);
VOID		 LfSelectXform	   (HWND, PTHR, USHORT);
VOID		 LfSelectPrim	   (HWND, PTHR, USHORT);
#endif

#ifdef INCL_LFDLG
/* lfdlg.c */
ULONG CALLBACK	 AboutDlg	   (HWND, USHORT, MPARAM, MPARAM);
ULONG CALLBACK	 TimerDelayDlg	   (HWND, USHORT, MPARAM, MPARAM);
ULONG CALLBACK	 BitmapParamsDlg   (HWND, USHORT, MPARAM, MPARAM);
ULONG CALLBACK	 SwitchDelayDlg    (HWND, USHORT, MPARAM, MPARAM);
ULONG CALLBACK	 LineAttrsDlg	   (HWND, USHORT, MPARAM, MPARAM);
ULONG CALLBACK	 MarkerAttrsDlg    (HWND, USHORT, MPARAM, MPARAM);
ULONG CALLBACK	 AreaAttrsDlg	   (HWND, USHORT, MPARAM, MPARAM);
ULONG CALLBACK	 ImageAttrsDlg	   (HWND, USHORT, MPARAM, MPARAM);
ULONG CALLBACK	 MiscAttrsDlg	   (HWND, USHORT, MPARAM, MPARAM);
ULONG CALLBACK	 DimensionsDlg	   (HWND, USHORT, MPARAM, MPARAM);
ULONG CALLBACK	 PtrPreferencesDlg (HWND, USHORT, MPARAM, MPARAM);
#endif

#ifdef INCL_LFDRAW
/* lfdraw.c */
VOID FAR cdecl	 LineFractalThread    (PTHR);
VOID		 LfUpdateAttrs	      (PTHR);
VOID		 LfComputeModelXforms (PTHR);
VOID		 LineFractal (PTHR, int, double, double, BOOL, PLINEFRAC);
VOID		 LfAddPoint	      (PTHR);
VOID		 LfDraw 	      (PTHR, BOOL);
VOID		 LfDrawPolyLine       (PTHR, BOOL);
VOID		 LfDrawPolyFillet     (PTHR, BOOL);
VOID		 LfDrawPolySpline     (PTHR, BOOL);
VOID		 LfDrawPolyPeano      (PTHR, BOOL);
VOID		 LfDrawPolyMarker     (PTHR, BOOL);
VOID		 LfClearRect	      (PTHR, PRECTL);
#endif

#ifdef INCL_LFFILE
/* lffile.c */
VOID		 LfReadFile  (HWND, PTHR);
BOOL		 LfReadBMP   (PTHR, PDLF);
VOID		 LfWriteFile (HWND, PTHR);
BOOL		 LfWriteBMP  (HPS, PBITMAPINFOHEADER, PDLF);
#endif

#ifdef INCL_LFUTIL
/* lfutil.c */
VOID		 MySetWindowUShort (HWND, USHORT, USHORT);
VOID		 MySetWindowLong   (HWND, USHORT, LONG);
VOID		 MySetWindowDouble (HWND, USHORT, double);
VOID		 MyGetWindowUShort (HWND, USHORT, PUSHORT);
VOID		 MyGetWindowLong   (HWND, USHORT, PLONG);
VOID		 MyGetWindowDouble (HWND, USHORT, PDBL);
VOID		 MyMessageBox	   (HWND, PSZ);
#endif
