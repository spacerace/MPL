#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bios.h>
#include <conio.h>
#include <graph.h>
#include <pgchart.h>
#include <stdarg.h>
#include "chrtdemo.h"

/* Structures for system configuration and chart environment. */
struct videoconfig vc;
chartenv ce;

/* Category variables.  */
short cCat;
char far * aCat[MAXVALUES];

/* Series variables.  */
short cSeries;
short far acSeries[MAXSERIES];
char far * aSeriesName[MAXSERIES];

/* Temporary holding array for all data. Data values for multi-series
 * bar, column, and line charts remain here. Data for other kinds of
 * charts are transferred to the arrays below.
 */
float far aValue[MAXSERIES][MAXVALUES];

/* Data values for single-series charts. First array is used for
 * bar, column, line, and pie. First and second are both used for
 * single-series scatter.
 */
float far axValue[MAXVALUES];
float far ayValue[MAXVALUES];

/* Data values for multi-series scatter charts.  */
float far axValueMS[MAXVALUES][MAXVALUES];
float far ayValueMS[MAXVALUES][MAXVALUES];

/* Exploded flags for pie chart.  */
short far aExplode[MAXVALUES];

/* Variables used to track control and screen position.  */
struct SCREENINFO
{
    int  top;                        /* Row under top form line      */
    int  bot;                        /* Row above bottom form line   */
    int  mid;                        /* Middle line of form          */
    int  help;                       /* Line number of help line     */
    int  mode;                       /* Current screen mode          */
    int  xMax;
    int  yMax;
    BOOL fColor;
} si;

/* These variables hold either the constants #defined for color
 * graphics adaptors (those that are formed: C_xxxxxCOLOR) or those
 * #defined for monochrome graphics adaptors (those that are formed:
 * M_xxxxxCOLOR).  They do NOT control the color of the presentation
 * graphics -- just the color of the menus and prompts.
 */
struct tagColor
{
    short InputColor;                /* Color for input requests     */
    short HiliteColor;               /* Color for prompt highlights  */
    short FormColor;                 /* Color for input forms/menus  */
    short TitleColor;                /* Color for titles             */
    short ErrorColor;                /* Color for error messages     */
    short InfoColor;                 /* Color for informations msgs. */
} co;


/* Constants that identify typefaces to ChooseFont. */

enum tagTypeFace
{
    COURIER,
    HELV,
    TMS_RMN,
    MODERN,
    SCRIPT,
    ROMAN
};


/* Flags to indicate whether to use imported or default data.  */
BOOL fDefault = TRUE;

/* Arrays of strings used by the Menu function. The first string is the
 * menu title. The next non-null strings are the menu selections. A null
 * string indicates the end of the list.
 */
char *pszAxes[] =
    { "Axis", "X Axis", "Y Axis", "" };

char *pszAxis[] =
    { "? Options", "Grid", "Axis Title", "Color",
      "Range Type", "Scale", "Tic Marks", "" };

char *pszAuto[] =
    { "Auto", "Auto", "Manual", "" };

char *pszBorder[] =
    { "Type", "Color", "Style", "" };

char *pszChartOpt[] =
    { "Options", "Screen Mode", "Windows", "Titles",
      "Axis (X and Y)", "Legend", "Fonts", "Reset", "" };

char *pszChartType[] =
    { "Type", "Bar", "Column", "Line", "Scatter", "Pie", "" };

char *pszChartWindow[] =
    { "Chart", "Size", "Color (Background)", "Border", "" };

char *pszDataWindow[] =
    { "Data", "Color (Background)", "Border", "" };

char * pszFontOpt[] =
    { "Font Options", "Change Typeface", "Set Character Size", "" };

char *pszJustify[] =
    { "Justify", "Left", "Center", "Right", "" };

char *pszLegendWindow[] =
    { "Options", "Place", "Text Color", "Size", "Color (Background)",
      "Border", "" };

char *pszMainMenu[] =
    { "Main Menu", "Demo", "View Chart", "Chart Type", "Options",
      "Show Chart Data", "Quit", "" };

char *pszPlace[] =
    { "Place", "Right", "Bottom", "Overlay", "" };

char *pszScale[] =
    { "Scale", "Low (Min)", "High (Max)", "Scale Factor", "Title", "" };

char *pszSize[] =
    { "Size", "Top", "Left", "Bottom", "Right", "" };

char *pszTic[] =
    { "Tic Type", "Interval", "Format", "Decimals", "" };

char *pszTitleOpt[] =
    { "", "Text", "Color", "Justify", "" };

char *pszTitles[] =
    { "Title", "Main Title", "Sub Title", "" };

char *pszTypeface[] =
    { "Type Faces", "Courier", "Helv", "Tms Rmn", "Modern", "Script",
           "Roman", "None", "" };

char *pszWindows[] =
    { "Window", "Chart Window", "Data Window", "" };

/* Sample data.  */
#define O_JUICE 0
#define I_TEA   1
#define H_CHOC  2
#define TEMPERATURE  3

char far * aQuarters[] =
    { "First", "Second", "Third", "Fourth" };

char far * aMonths[] =
    { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
      "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

float far aSales[3][12] =
{
    {  3.6,  3.2,  3.3,  3.4,  3.1,  2.9,  3.0,  3.6,  3.2,  3.3,  3.5,  3.9 },
    {  1.0,  1.3,  1.4,  1.7,  2.2,  2.9,  2.9,  3.1,  2.6,  1.8,  1.1,  1.2 },
    {  2.4,  2.3,  2.0,  1.6,  1.3,  1.0,  0.9,  0.8,  1.1,  1.4,  1.9,  2.5 }
};
  
float far aTemperature[12] =
    {  2.9,  3.2,  3.9,  4.8,  6.0,  6.5,  7.0,  7.2,  6.0,  4.7,  4.1,  3.0 };

char far * aSalesTitles[] =
    { "Orange Juice Sales", "Iced Tea Sales", "Hot Chocolate Sales" };

char *TempTitle = "Average Temperature";

int main()
{
    Initialize();
    MainMenu();

    /* Reset the video mode and screen colors prior to leaving. */
    _setvideomode( _DEFAULTMODE );
    _settextcolor( co.InfoColor );
    _clearscreen( _GCLEARSCREEN );

    return 0;
}

/*  Axes - Selects X or Y axis.
 *
 *  Params: none
 */
void Axes()
{
    int iChoice;
    static axistype *patAxis[2] = { &ce.xaxis, &ce.yaxis };

    /* Get menu choice and call appropriate axis Menu. */
    PushTitle( pszAxes[0] );
    Help( "Choose 'X' or 'Y' Axis", co.InputColor );
    while( (iChoice = Menu( pszAxes )) != ESCAPE )
    {
        /* Modify axis title, depending on choice. */
        pszAxis[0][0] = (--iChoice == 0) ? 'X' : 'Y';

        /* Obtain axis information for appropriate axis. */
        Axis( patAxis[iChoice] );
    }
    PopTitle();
}

/*  Axis - Selects axis options.
 *
 *  Params: pat - Pointer to axistype variable
 */
void Axis( axistype *pat )
{
    int iChoice;

    PushTitle( pszAxis[0] );
    while( (iChoice = Menu( pszAxis )) != ESCAPE )
    {

        /* Get Axis option.  */
        switch( iChoice )
        {
            case 1:
                /* Grid or not? */
                iChoice = BlankMenu( "Grid", "Grid", "No Grid" );
                switch( iChoice )
                {

                    case 1:
                        /* If yes, set grid flag and get the grid style. */
                        pat->grid = TRUE;
                        Help( "Enter a number in the range 0-10.",
                            co.InputColor );
                        pat->gridstyle =
                            InputInt( "Grid Style? ", pat->gridstyle, 0, 10 );
                        break;

                    case 2:
                        /* If no, clear grid flag.  */
                        pat->grid = FALSE;
                }
                PopTitle();
                break;

            case 2:
                /* Select axis title options. */
                pszTitleOpt[0] = "Axis Title";
                TitleOpt( &pat->axistitle );
                break;

            case 3:
                /* Select color. */
                Help( "Enter a number in the range 0-15.", co.InputColor );
                pat->axiscolor =
                    InputInt( "Axis Color? ", pat->axiscolor, 0, 15 );
                break;

            case 4:
                /* Get the axis range.  */
                AxisRange( pat );
                break;

            case 5:
                /* Get the axis scale.  */
                AxisScale( pat );
                break;

            case 6:
                /* Get axis tic mark options.  */
                AxisTics( pat );
                break;

        }
    }
    PopTitle();
}

/*  AxisRange - Selects range for an axis.
 *
 *  Params: pat - pointer to axistype variable
 */
void AxisRange( axistype *pat )
{
    int iChoice;

    iChoice = BlankMenu( "Range Type", "Normal", "Log" );
    switch( iChoice )
    {
        case 1:
            /* Set range type to linear.  */
            pat->rangetype = _PG_LINEARAXIS;
            break;

        case 2:
            /* Set range type to log, then query for log base.  */
            pat->rangetype = _PG_LOGAXIS;
            Help( "Enter a value greater than or equal 2.", co.InputColor );
            pat->logbase = InputInt( "Log base? ", (int)pat->logbase, 2, 0 );
            break;
    }
    PopTitle();
}

/*  AxisScale - Selects scale options for an axis.
 *
 *  Params: pat - pointer to axistype variable
 */
void AxisScale( axistype *pat )
{
    int iChoice;

    PushTitle( pszAuto[0] );
    iChoice = Menu( pszAuto );
    switch( iChoice )
    {

        case 1:
            /* Set AutoScale flag.  */
            pat->autoscale = TRUE;
            break;

        case 2:

            /* Clear AutoScale flag and get scale options.  */
            pat->autoscale = FALSE;
            PushTitle( pszScale[0] );
            while( (iChoice = Menu( pszScale )) != ESCAPE )
            {

                switch( iChoice )
                {

                    case 1:  
                        /* Query for scale minimum.  */
                        Help( "Enter the range minimum value.", co.InputColor );
                        pat->scalemin =
                            InputInt( "Minimum? ", pat->scalemin, 1, 0 );
                        break;

                    case 2:  
                        /* Query for scale maximum.  */
                        Help( "Enter the range maximum value.", co.InputColor );
                        pat->scalemax =
                            InputInt( "Maximum? ", pat->scalemax, 1, 0 );
                        break;

                    case 3:
                        /* Query for scale factor.  */
                        Help( "Enter scale factor (must be 1 or greater).",
                                 co.InputColor );
                        pat->scalefactor =
                            InputInt( "Scale Factor? ", pat->scalefactor,
                                       1, 0 );
                        break;

                    case 4:  
                        /* Modify scale title, then use menu to get
                         * title options.
                         */
                        pszTitleOpt[0] = "Scale Title";
                        TitleOpt( &pat->scaletitle );

                }
            }
            PopTitle();
    }
    PopTitle();
}

/*  AxisTics - Selects tic options for an axis.
 *
 *  Params: pat - pointer to axistype variable
 */
void AxisTics( axistype *pat )
{
    int iChoice;

    PushTitle( pszTic[0] );
    while( (iChoice = Menu( pszTic )) != ESCAPE )
    {
        switch( iChoice )
        {

            case 1:
                /* Query for tic interval.  */
                Help( "Enter distance in data units.", co.InputColor );
                pat->ticinterval =
                    InputFloat( "Distance between tic marks? ",
                               pat->ticinterval );
                pat->autoscale = FALSE;
                break;

            case 2:
                /* Query for tic format.  */
                iChoice = BlankMenu( "Tic Format", "Normal", "Log" );
                if( iChoice != ESCAPE )
                    pat->ticformat = iChoice;
                break;

            case 3:
                /* Query for number of decimal places per tic.  */
                pat->ticdecimals =
                    InputInt( "Enter decimal places (0 to 9). ",
                               pat->ticdecimals, 0, 9 );
                pat->autoscale = FALSE;
                break;
        }

    }
    PopTitle();
}

/*  Border - Specifies border information for a window.
 *
 *  Params: pwt - Pointer to windowtype variable
 */
void Border( windowtype *pwt )
{
    int iChoice;

    /* Ask whether a border is wanted.  */
    iChoice = BlankMenu( "Border", "Border", "No Border" );
    switch( iChoice )
    {

        case 1:

            /* If border, set Border flag and query for border options.  */
            pwt->border= TRUE;
            PushTitle( pszBorder[0] );
            while( (iChoice = Menu( pszBorder )) != ESCAPE )
            {
                switch( iChoice )
                {
                    case 1:
                        /* Query for border color.  */
                        Help( "Enter a color in the range 0-15.",
                            co.InputColor );
                        pwt->bordercolor =
                            InputInt( "Border color? ",
                                       pwt->bordercolor, 0, 15 );
                        break;

                    case 2:
                        /* Query for border style.  */
                        Help( "Enter a style in the range 0-10.", co.InputColor );
                        pwt->borderstyle =
                            InputInt( "Border style? ",
                                       pwt->borderstyle, 0, 10 );
                }
            }
            PopTitle();
            break;

        case 2:
            /* If no border, clear Border flag.  */
            pwt->border= FALSE;
    }
    PopTitle();
}

/*  ChangeTypeface - Allow the user to specify a new type face.
 *
 *  Params: None
 */

void ChangeTypeface()
{
    int iChoice;
    struct _fontinfo fd;
    static BOOL FontsRegistered = FALSE;    /* state of font package */

    /* Get menu choice and call appropriate axis Menu. */
    PushTitle( pszFontOpt[0] );
    Help( "Choose one of the type faces listed.", co.InputColor );

    if( (iChoice = Menu( pszTypeface )) != ESCAPE )
    {
        /* If the user wants the system font, unregister the other fonts. */
        if( iChoice == 7 )
        {
            _unregisterfonts();
            FontsRegistered = FALSE;
        }

        /* If the user wants any font but the system font, make sure the
         *   fonts are registered.
         */
        else
        {
            if( !FontsRegistered )
            {
                if( _registerfonts( "*.FON" ) < 0 )
                {
                    ErrorMsg( "Unable to register fonts" );
                    return;
                }
                else
                    FontsRegistered = TRUE; /* Reset the state to registered */
            }

            /* Get the previous font settings */
            _getfontinfo( &fd );

            ChooseFont( iChoice - 1, fd.pixheight );
        }
    }

    PopTitle();
}

/*  ChartOptions - Gets chart options.
 *
 *  Params: None
 */
void ChartOptions()
{
    int iChoice;

    PushTitle( pszChartOpt[0] );
    while( (iChoice = Menu( pszChartOpt )) != ESCAPE )
    {

        /* Get chart options.  */
        switch( iChoice )
        {

            /* Branch to the appropriate menu.  */
            case 1:
                ScreenMode();
                break;

            case 2:
                Windows();
                break;

            case 3:
                Titles();
                break;

            case 4:
                Axes();
                break;

            case 5:
                Legend();
                break;

            case 6:
                FontOptions();
                break;

            case 7:
                ResetOptions();
                break;

        }
    }
    PopTitle();
}

/*  ChartType - Gets chart type.
 *
 *  Params: None
 */
void ChartType()
{
    int iChoice;

    /* Get chart type.  */
    PushTitle( pszChartType[0] );
    iChoice = Menu( pszChartType );

    if( iChoice != ESCAPE )
    {
        /* Set the chart type, and get the chart style.  */
        ce.charttype = iChoice;
        switch( iChoice )
        {

            case 1:
            case 2:
                iChoice = BlankMenu( "Style", "Plain Bars", "Stacked Bars" );
                break;

            case 3:
            case 4:
                iChoice = BlankMenu( "Style", "Lines-Points", "Points Only" );
                break;

            case 5:
                iChoice = BlankMenu( "Style", "Percent", "No Percent" );
        }

        if( iChoice != ESCAPE)
            ce.chartstyle = iChoice;

        /* Set default data without changing environment defaults.  */
        DefaultData( ce.charttype, ce.chartstyle, FALSE );
        PopTitle();
    }
    PopTitle();
}

/*  ChooseFont - Chooses a font from the font library.
 *
 *  Params: WhichFont - A member of the set [COURIER, HELV, TMS_RMN,
 *                        MODERN, SCRIPT, ROMAN]
 *          Height    - The desired height of the text (in pixels)
 */

void ChooseFont( int WhichFont, int Height )
{
    static char *FontIds[] =
    {
        "courier", "helv", "tms rmn", "modern", "script", "roman"
    };
    char SetCommand[21];


    /* Construct the command to send to _setfont. */

    sprintf( SetCommand, "t'%s'h%dw0b", FontIds[WhichFont], Height );

    if( _setfont( SetCommand ) )
    {
        _outtext( "Could not set font to " );
        _outtext( FontIds[WhichFont] );
        _outtext( "." );
        getch();
    }
}


/*  ChartWindow - Gets chart window information.
 *
 *  Params: None
 */
void ChartWindow()
{
    int iChoice;

    PushTitle( pszChartWindow[0] );
    while( (iChoice = Menu( pszChartWindow )) != ESCAPE )
    {

        /* Get window options.  */
        switch( iChoice )
        {

            case 1:
                /* Get window size.  */
                WindowSize( &ce.chartwindow );
                break;

            case 2:
                /* Query for background color.  */
                Help( "Enter a number in the range 0-15", co.InputColor );
                ce.chartwindow.background =
                    InputInt( "Background Color? ", ce.chartwindow.background,
                              0, 15 );
                break;

            case 3:

                /* Get border options.  */
                Border( &ce.chartwindow );

        }
    }
    PopTitle();
}

/*  ClearData - Clears category and value data.
 *
 *  Params: fConfirm - flag specifying whether to query for confirmation
 */
void ClearData( BOOL fConfirm )
{
    char chResponse = 'Y';
    int iCat;

    WrtForm( 18 );

    /* Query for confirmation.  */
    if( fConfirm )
        chResponse = InputCh( "Are you sure? ", "YN\x1b" );

    if( chResponse == 'Y' )
    {

        /* Clear all relevant data.  */
        for( iCat = 0; iCat < cCat; iCat++ )
            aCat[iCat] = NULL;
        cCat = 0;
        cSeries = 0;
    }
}

/*  DataWindow - Geta data window information.
 *
 *  Params: None
 */
void DataWindow()
{
    int iChoice;

    PushTitle( pszDataWindow[0] );
    while( (iChoice = Menu( pszDataWindow )) != ESCAPE )
    {

        /* Get data window menu options.  */
        switch( iChoice )
        {

            case 1: 
                /* Query for background color.  */
                Help( "Enter a number in the range 0-15", co.InputColor );
                ce.datawindow.background =
                    InputInt( "Background Color? ",
                               ce.datawindow.background,
                               0, 15 );
                break;

            case 2:
                /* Get border options.  */
                Border( &ce.datawindow );
                break;

        }
    }
    PopTitle();
}

/*  DefaultData - Initializes default data for each kind of chart.
 *
 *  Params: iType - Chart type to be initialized
 *          iStyle - Chart style
 *          fClear - Signal to clear all defaults
 */
void DefaultData( short iType, short iStyle, BOOL fClear )
{
    int iCat, iValue, iSubValue, iSeries;

    /* Call default chart to clear old values.  */
    if( fClear )
        _pg_defaultchart( &ce, iType, iStyle );

    /* Initialize category titles.  */
    cCat = 12;
    for( iCat = 0; iCat < cCat; iCat++ )
        aCat[iCat] = aMonths[iCat];

    switch( ce.charttype )
    {

        /* Initialize data for each chart type.  */
        case _PG_BARCHART:
        
            strcpy( ce.maintitle.title, "Orange Juice and Iced Tea Sales" );
            cSeries = 2;
            for( iSeries = 0; iSeries < cSeries; iSeries++ )
            {
                aSeriesName[iSeries] = aSalesTitles[iSeries];
                acSeries[iSeries] = cCat;
                for( iValue = 0; iValue < cCat; iValue++ )
                    aValue[iSeries][iValue]  = (float)aSales[iSeries][iValue];
            }
            break;

        case _PG_COLUMNCHART:
       
            strcpy( ce.maintitle.title, "Orange Juice Sales" );
            cSeries = 1;
            for( iSeries = 0; iSeries < cSeries; iSeries++ )
            {
                aSeriesName[iSeries] = aSalesTitles[iSeries];
                acSeries[iSeries] = cCat;
                for( iValue = 0; iValue < cCat; iValue++ )
                    aValue[iSeries][iValue]  = (float)aSales[iSeries][iValue];
            }
            break;

        case _PG_LINECHART:

            strcpy( ce.maintitle.title, "Beverage Sales" );
            cSeries = 3;
            for( iSeries = 0; iSeries < cSeries; iSeries++ )
            {
                aSeriesName[iSeries] = aSalesTitles[iSeries];
                acSeries[iSeries] = cCat;
                for( iValue = 0; iValue < cCat; iValue++ )
                    aValue[iSeries][iValue] = (float)aSales[iSeries][iValue];
            }
            break;

        case _PG_SCATTERCHART:

            strcpy( ce.maintitle.title,
                    "Average Temperature Compared to Beverage Sales" );
            /* ce.chartstyle = _PG_POINTONLY; */
            cSeries = 4;

            aSeriesName[0] = aSalesTitles[I_TEA];
            aSeriesName[2] = aSalesTitles[H_CHOC];
            acSeries[0] = acSeries[1] = acSeries[2] = acSeries[3] = 12;
            for( iValue = 0; iValue < 12; iValue++ )
            {
                aValue[0][iValue]  = (float)aSales[I_TEA][iValue] ;
                aValue[1][iValue]  = (float)aSales[TEMPERATURE][iValue] ;
                aValue[2][iValue]  = (float)aSales[H_CHOC][iValue] ;
                aValue[3][iValue]  = (float)aSales[TEMPERATURE][iValue] ;
            }
            break;
           
        case _PG_PIECHART:
        default:

            strcpy( ce.maintitle.title, "Iced Tea Sales" );
            cCat = 4;
            for( iCat = 0; iCat < cCat; iCat++ )
                aCat[iCat] = aQuarters[iCat];

            cSeries = 1;
            aSeriesName[0] = aSalesTitles[I_TEA];
            acSeries[0] = cCat;

            for( iValue = 0; iValue < cCat; iValue++ )
            {
                aValue[0][iValue] = 0.0;
                for( iSubValue = 0; iSubValue < 3; iSubValue++ )
                    aValue[0][iValue] += (float)aSales[I_TEA][iSubValue * iValue];
            }
            aExplode[3] = 1;
            break;
    }
}

/*  Demo - Displays a series of sample charts.
 *
 *  Params: None
 */
void Demo()
{
    int  cValue;
    palettetype palette_struct;

    /* Display the sample data in spreadsheet form. */
    ShowSampleData();

    DefaultData( _PG_PIECHART, _PG_NOPERCENT, TRUE );

    /* Set video mode and draw charts. For each chart, set default
     * data and modify any desired environment fields. If error,
     * terminate demo.
     */
    _setvideomode( si.mode );

    cValue = 4;
    strcpy( ce.subtitle.title, "Default Pie Chart" );
    if( ViewChart() )
        return;
    else
        _clearscreen( _GCLEARSCREEN );

    strcpy( ce.subtitle.title, "Customized Pie Chart" );
    ce.chartstyle = _PG_PERCENT;
    ce.legend.place = _PG_BOTTOM;
    if (si.fColor)
    {
        ce.maintitle.titlecolor = ce.subtitle.titlecolor = 0;
        ce.chartwindow.background = 1;
        ce.datawindow.background = ce.legend.legendwindow.background = 6;
        ce.legend.textcolor = 1;
    }
    if( ViewChart() )
        return;
    else
        _clearscreen( _GCLEARSCREEN );

    cValue = 12;
    DefaultData( _PG_BARCHART, _PG_PLAINBARS, TRUE );
    strcpy( ce.subtitle.title, "Default Bar Chart" );
    if( ViewChart() )
        return;
    else
        _clearscreen( _GCLEARSCREEN );

    strcpy( ce.subtitle.title, "Customized Stacked Bar Chart" );
    strcpy( ce.xaxis.axistitle.title, "Sales in Thousands" );
    strcpy( ce.yaxis.axistitle.title, "Month" );
    ce.chartstyle = _PG_STACKEDBARS;
    ce.legend.place = _PG_RIGHT;
    ce.xaxis.ticdecimals = 2;
    if (si.fColor)
    {
        ce.maintitle.titlecolor = ce.subtitle.titlecolor = 12;
        ce.chartwindow.background = 7;
        ce.datawindow.background = 8;
        ce.legend.textcolor = 0;
        ce.legend.legendwindow.background = 8;
        ce.legend.autosize = FALSE;
        ce.legend.legendwindow.y1 = vc.numypixels - 85;
        ce.legend.legendwindow.y2 = vc.numypixels - 45;
    }
    if( ViewChart() )
        return;
    else
        _clearscreen( _GCLEARSCREEN );

    DefaultData( _PG_COLUMNCHART, _PG_PLAINBARS, TRUE );
    strcpy( ce.subtitle.title, "Default Column Chart" );
    if( ViewChart() )
        return;
    else
        _clearscreen( _GCLEARSCREEN );

    strcpy( ce.subtitle.title, "Customized Column Chart" );
    strcpy( ce.xaxis.axistitle.title, "Month" );
    strcpy( ce.yaxis.axistitle.title, "Sales in Thousands" );
    ce.legend.place = _PG_BOTTOM;
    if (si.fColor)
    {
        ce.maintitle.titlecolor     = 0;
        ce.subtitle.titlecolor     = 0;
        ce.chartwindow.background = 8;
        ce.datawindow.background = 2;
        ce.legend.legendwindow.background = 10;
    }
    if( ViewChart() )
        return;
    else
        _clearscreen( _GCLEARSCREEN );

    DefaultData( _PG_LINECHART, _PG_POINTANDLINE, TRUE );
    strcpy( ce.subtitle.title, "Default Line Chart" );
    if( ViewChart() )
        return;
    else
        _clearscreen( _GCLEARSCREEN );

    strcpy( ce.subtitle.title, "Customized Line Chart" );
    strcpy( ce.xaxis.axistitle.title, "Month" );
    strcpy( ce.yaxis.axistitle.title, "Sales in Thousands" );
    ce.legend.place = _PG_RIGHT;
    if (si.fColor)
    {
        ce.maintitle.titlecolor = 1;
        ce.subtitle.titlecolor = 1;
        ce.chartwindow.background = 3;
        ce.datawindow.background = 7;
        ce.legend.legendwindow.background = 7;
        ce.legend.autosize = FALSE;
        ce.legend.legendwindow.y1 = vc.numypixels - 85;
        ce.legend.legendwindow.y2 = vc.numypixels - 45;
    }
    if( ViewChart() )
        return;
    else
        _clearscreen( _GCLEARSCREEN );

    DefaultData( _PG_SCATTERCHART, _PG_POINTONLY, TRUE );
    strcpy( ce.subtitle.title, "Default Scatter Chart" );
    if( ViewChart() )
        return;
    else
        _clearscreen( _GCLEARSCREEN );

    cSeries = 4;
    strcpy( ce.subtitle.title, "Customized Scatter Chart" );
    strcpy( ce.xaxis.axistitle.title, "Sales in Thousands" );
    strcpy( ce.yaxis.axistitle.title, "Average Temperature" );
    ce.legend.place = _PG_RIGHT;
    if (si.fColor)
    {
        ce.maintitle.titlecolor = 0;
        ce.subtitle.titlecolor = 0;
        ce.chartwindow.background = 4;
        ce.datawindow.background = 8;
        ce.legend.legendwindow.background = 8;
        ce.legend.autosize = FALSE;
        ce.legend.legendwindow.y1 = vc.numypixels - 85;
        ce.legend.legendwindow.y2 = vc.numypixels - 45;
    }
    if( ViewChart() )
        return;
    else
        _clearscreen( _GCLEARSCREEN );

    DefaultData( _PG_BARCHART, _PG_PERCENT, TRUE );
}

/*  FindVideoMode - Finds the "best" video mode for the adaptor in use.
 *
 *  Params: vc - structure of type struct videoconfig
 *
 *  Returns: Best mode
 */
int FindVideoMode( struct videoconfig vc )
{
    switch( vc.adapter )
    {
        case _CGA:
        case _OCGA:
            return _HRESBW;
        case _EGA:
		case _OEGA:
			return( vc.monitor == _MONO ) ? _ERESNOCOLOR : _ERESCOLOR;
        case _VGA:
        case _OVGA:
        case _MCGA:
            return _VRES16COLOR;
        case _HGC:
            return _HERCMONO;
        default:
            return _DEFAULTMODE;
    }
}

/*  FontOptions - Allows the user to modify the font used for display.
 *
 *  Params: None
 */

void FontOptions()
{
    int iChoice;
    int iTypeSize;
    int iFaceIndex;
    struct _fontinfo fd;

    /* Get menu choice and call appropriate axis Menu. */
    PushTitle( pszFontOpt[0] );

    while( (iChoice = Menu( pszFontOpt )) != ESCAPE )
    {
        /* Get the current font information. */
        _getfontinfo( &fd );

        switch( iChoice )
        {
            /* Change Typeface. */
            case 1:
                ChangeTypeface();
                break;

            /* Change Type Size. */
            case 2:
                iTypeSize = InputInt( "Enter a type size. ", fd.pixheight,
                                         8, 128 );

                for( iFaceIndex = 0; iFaceIndex < 6; ++iFaceIndex )
                    if( !strcmpi( fd.facename, pszTypeface[iFaceIndex + 1] ) )
                        break;

                ChooseFont( iFaceIndex, iTypeSize );
                break;

            default:
                break;
        }
    }
    PopTitle();
}

/*  Initialize - Does various initialization tasks.
 *
 *  Params: None
 */
void Initialize( void )
{
    int iSeries, iValue;

    /* Initialize all value arrays to missing.  */
    for( iSeries = 0; iSeries < MAXSERIES; iSeries++ )
    {

        axValue[iSeries] = _PG_MISSINGVALUE;
        ayValue[iSeries] = _PG_MISSINGVALUE;

        for( iValue = 0; iValue < MAXVALUES; iValue++ )
            aValue[iSeries][iValue] = _PG_MISSINGVALUE;

        for( iValue = 0; iValue < MAXVALUES; iValue++ )
        {
            axValueMS[iSeries][iValue] = _PG_MISSINGVALUE;
            ayValueMS[iSeries][iValue] = _PG_MISSINGVALUE;
        }
    }

    /* Initialize zero sets. */
    cSeries = 0;

    /* Initialize default chart environment, screen mode, and data.  */
    _pg_initchart();
    _getvideoconfig( &vc );

    /* Find the best available mode for display.
     * Don't set 256 color, medium resolution (_MRES256COLOR).
     */
    si.mode = FindVideoMode( vc );

    if( si.mode == _TEXTMONO )
    {
        _clearscreen( _GCLEARSCREEN );
        _outtext( "No graphics available.  Can't run chart demo." );
        exit( 1 );
    }

    SetDisplayColors();

    SetGraphMode( si.mode );
    DefaultData( _PG_BARCHART, _PG_PLAINBARS, TRUE );

    _setvideomode( _DEFAULTMODE );


}

/*  Justify - Gets title justification option.
 *
 *  Params: Pointer to titletype variable
 */
void Justify( titletype *ptt )
{
    int iChoice;

    PushTitle( pszJustify[0] );
    iChoice = Menu( pszJustify );
    switch( iChoice )
    {

        /* Set justification.  */
        case 1:
        case 2:
        case 3:
            ptt->justify = iChoice;
    }
    PopTitle();
}

/*  Legend - Asks whether a legend is desired, and if so, gets
 *  legend options.
 *
 *  Params: None
 */
void Legend()
{
    int iChoice;

    /* Is legend desired?  */
    iChoice = BlankMenu( "Legend", "Legend", "No Legend" );
    switch( iChoice )
    {
        case 1:
            /* If legend, set legend flag and get options.  */
            ce.legend.legend = TRUE;
            PushTitle( pszLegendWindow[0] );
            do
            {
                iChoice = Menu( pszLegendWindow );
                switch( iChoice )
                {

                    case 1:
                        /* Get legend place.  */
                        LegendPlace();
                        break;

                    case 2:
                        /* Query for legend color.  */
                        Help( "Enter a number in the range 0-15.", co.InputColor );
                        ce.legend.textcolor =
                            InputInt( "Text color? ",
                                       ce.legend.textcolor,
                                       0, 15 );
                        break;

                    case 3:
                        /* Get auto or manual sizing.  */
                        PushTitle( "Auto Legend" );
                        iChoice = Menu( pszAuto );

                        /* Set or clear the autosize flag. If manual
                         * sizing was selected, get legend size.
                         */
                        switch( iChoice )
                        {
                            case 1:
                                ce.legend.autosize = TRUE;
                                break;

                            case 2:
                                ce.legend.autosize = FALSE;
                                WindowSize( &ce.legend.legendwindow );
                        }
                        PopTitle();
                        break;

                    case 4:
                        /* Query for background color.  */
                        Help( "Type a number in the range 0-15.", co.InputColor );
                        ce.legend.legendwindow.background =
                            InputInt( "Background color? ",
                                       ce.legend.legendwindow.background,
                                       0, 15 );
                        break;

                    case 5:
                        /* Get border options for legend window.  */
                        Border( &ce.legend.legendwindow );
                }

            } while( iChoice != ESCAPE );
            PopTitle();
            break;

        case 2:
            /* If no legend wanted, clear flag.  */
            ce.legend.legend = FALSE;

    }
    PopTitle();
}

/*  LegendPlace - Gets legend placement option.
 *
 *  Params: None
 */
void LegendPlace()
{
    int iChoice;

    /* Get legend placement.  */
    PushTitle( pszPlace[0] );
    iChoice = Menu( pszPlace );
    switch( iChoice )
    {

        case 1:
            ce.legend.place = _PG_RIGHT;
            break;

        case 2:
            ce.legend.place = _PG_BOTTOM;
            break;

        case 3:
            ce.legend.place = _PG_OVERLAY;
    }
    PopTitle();
}

/*  MainMenu - Manages the main menu.
 *
 *  Params: None
 */
void MainMenu( void )
{
    int iChoice;
    char chResponse = 'Y';
    char chVerify;

    PushTitle( pszMainMenu[0] );
    do
    {
        /* If the user selects Quit, iChoice will contain 6.  If the
         *   user presses ESCAPE, iChoice will be ESCAPE, which is
         *   equal to 27.  In any case, we can test both conditions
         *   by checking to see whether iChoice is less than 6.
         */
        while( (iChoice = Menu( pszMainMenu )) < 6 )
        {
            /* Get main menu selection.  */
            switch( iChoice )
            {

                case 1:
                    /* Display demo charts.  */
                    Demo();
                    _setvideomode( _DEFAULTMODE );
                    break;

                case 2:
                    /* Set graphics video mode, display current chart,
                     * and restore text video mode.
                     */
                    _setvideomode( si.mode );
                    ViewChart();
                    _setvideomode( _DEFAULTMODE );
                    break;

                case 3:
                    /* Get chart type and style.  */
                    ChartType();
                    break;

                case 4:
                    /* Get chart options.  */
                    ChartOptions();
                    break;
                case 5:
                    /* Show chart data. */
                    ShowChartData();
                    break;

            }

        }

        /* If the user is trying to leave the program using the ESCAPE
         *   key, we must verify the choice.  This is done to prevent
         *   an eager typist from pressing ESCAPE one time too often
         *   and exiting at an unanticipated point.
         */
        if( iChoice == ESCAPE )
        {
            Help( "Press \"Q\" to Actually Quit", co.InputColor );

            putchar( BEEP );
            _settextposition( si.help - 1, 32 );
            chVerify = getch();
            if( tolower( chVerify ) != 'q' )
                iChoice = 0;
            else
                iChoice = 6;
        }

    } while( iChoice != 6 );
    PopTitle();
}

/*  ResetOptions - After confirmation, resets chart options to default.
 *
 *  Params: None
 */
void ResetOptions()
{
    char chResponse;

    /* Prompt for confirmation before setting default environment.  */
    ClrForm();
    Help( "Type  'Y' to reset all options, 'N' to keep them.", co.InputColor );
    chResponse = InputCh( "Are you sure? ", "YN\x1b" );
    if( chResponse == 'Y' )
        _pg_defaultchart( &ce, 1, 1 );

}

/*  ScreenMode - Gets a new screen mode.
 *
 *  Params: None
 */
void ScreenMode()
{
    int iMode, i;
    char szTmp[80], szHlp[80];
    static int iLegal[5][11] =
    {
        { 3, 4, 5, 6 },
        { 4, 4, 5, 6, 64 },
        { 4, 4, 5, 6, 19 },
        { 7, 4, 5, 6, 13, 14, 15, 16 },
        { 10, 4, 5, 6, 13, 14, 15, 16, 17, 18, 19 }
    };
    int iAdaptor;

    PushTitle( "Screen Mode" );

    /* Show appropriate help line for adaptor.  */
    switch( vc.adapter )
    {
        case _HGC:
            PopTitle();
            return;
        case _CGA:
            iAdaptor = 0;
            break;
        case _OCGA:
            iAdaptor = 1;
            break;
        case _MCGA:
            iAdaptor = 2;
            break;
        case _EGA:
        case _OEGA:
            if( vc.adapter == _MONO )
            {
                PopTitle();
                return;
            }
            else
                iAdaptor = 3;
            break;
        case _VGA:
        case _OVGA:
            iAdaptor = 4;
            break;
    }

    /* Form the help line (which gives the choices legal for
     * the adaptor sensed in the user's machine).
     */
    for( iMode = 0, szHlp[0] = '\0'; iMode <= iLegal[iAdaptor][0]; ++iMode )
    {
        if( iMode == 0 )
            strcpy( szTmp, "Enter " );
        else if( iMode < iLegal[iAdaptor][0] )
            sprintf( szTmp, "%d, ", iLegal[iAdaptor][iMode] );
        else
            sprintf( szTmp, "or %d", iLegal[iAdaptor][iMode] );
        strcat( szHlp, szTmp );
    }

    WrtForm( 18 );
    Help( szHlp, co.InputColor );

    /* Query for screen mode. */
    for( ;; )
    {
        iMode = InputInt( "Screen Mode? ", si.mode, 1, 64 );
        for( i = 1; i <= iLegal[iAdaptor][0]; ++i ) /* Test legal values    */
            if( iMode == iLegal[iAdaptor][i] )      /* If a match is found  */
                break;                              /* Terminate for loop   */
        if( iMode == iLegal[iAdaptor][i] )          /* If it's a match,     */
            break;                                  /* terminate do loop,   */
        else                                        /* otherwise BEEP, and  */
            putchar( BEEP );                        /* solicit correct data */
    }

    PopTitle();
    if( SetGraphMode( iMode ) )
        _setvideomode( _DEFAULTMODE );
    else
        ShowError( _PG_BADSCREENMODE );

    /* Force rescaling of the chart by resetting the window
     * rectangles for the chart and data windows to zero size.
     */
    ce.chartwindow.x1 = ce.chartwindow.x2 = ce.chartwindow.y1 =
        ce.chartwindow.y2 = 0;
    ce.datawindow = ce.chartwindow;
}

/*  SetGraphMode - Tests the specified graphics mode and sets the xMax
 *  and yMax values in the si (Screen Information) structure.
 *
 *  Params: mode number
 *
 *  Return: FALSE if mode invalid, TRUE if valid
 */
BOOL SetGraphMode(int mode)
{
    if (!_setvideomode( mode ) )
        return FALSE;
    else
    {
        _getvideoconfig ( &vc );
        if( !vc.numxpixels )
            return FALSE;
        si.xMax = vc.numxpixels;
        si.yMax = vc.numypixels;
        si.mode = mode;

        /* Set flag to indicate whether multiple colors are available.  */
        si.fColor = iscolor( mode );

        return TRUE;
    }
}

/*  ShowChartData - Displays the data in the chart environment.
 *
 *  Params: None
 */
void ShowChartData()
{
    int iRow = 2;
    static char *szContinue =
        "Press any key to continue, ESC to return to the menu.";

    _clearscreen( _GCLEARSCREEN );
    SprintAt( iRow++, 1, "short      charttype =  %d", ce.charttype );
    SprintAt( iRow++, 1, "short      chartstyle = %d", ce.chartstyle );
    SprintAt( iRow++, 1, "windowtype chartwindow =" );
    iRow = ShowWindowType( iRow, 1, ce.chartwindow );
    SprintAt( iRow++, 1, "windowtype datawindow =" );
    iRow = ShowWindowType( iRow, 1, ce.datawindow );
    SprintAt( ++iRow, 1, szContinue );
    if( getch() == ESCAPE )
        return;

    iRow = 2;
    _clearscreen( _GCLEARSCREEN );
    SprintAt( iRow++, 1, "titletype  maintitle =" );
    iRow = ShowTitleType( iRow, 1, ce.maintitle );
    SprintAt( iRow++, 1, "titletype   subtitle =" );
    iRow = ShowTitleType( iRow, 1, ce.subtitle );
    SprintAt( ++iRow, 1, szContinue );
    if( getch() == ESCAPE )
        return;

    iRow = 2;
    _clearscreen( _GCLEARSCREEN );
    SprintAt( iRow++, 1, "axistype       xaxis =" );
    iRow = ShowAxisType( iRow, 1, ce.xaxis );
    SprintAt( ++iRow, 1, szContinue );
    if( getch() == ESCAPE )
        return;

    iRow = 2;
    _clearscreen( _GCLEARSCREEN );
    SprintAt( iRow++, 1, "axistype       yaxis =" );
    iRow = ShowAxisType( iRow, 1, ce.yaxis );
    SprintAt( ++iRow, 1, szContinue );
    if( getch() == ESCAPE )
        return;

    iRow = 2;
    _clearscreen( _GCLEARSCREEN );
    SprintAt( iRow++, 1, "legendtype     legend =" );
    iRow = ShowLegendType( iRow, 1, ce.legend );
    SprintAt( ++iRow, 1, "Press any key to continue . . ." );
    getch();
}

/*  ShowAxisType - Displays data in a variable of type "axistype".
 *
 *  Params: iRow - Row at which to start
 *          iCol - Column from which to indent
 *          theAxis - Variable of type "axistype" to display
 *
 *  Return: Next available row
 */
int ShowAxisType( int iRow, int iCol, axistype theAxis )
{
    SprintAt( iRow++, iCol + 5, "short        .grid = %d", theAxis.grid );
    SprintAt( iRow++, iCol + 5, "short   .gridstyle = %d", theAxis.gridstyle );
    SprintAt( iRow++, iCol + 5, "titletype axistitle=" );
    iRow = ShowTitleType( iRow, iCol + 5, theAxis.axistitle );
    SprintAt( iRow++, iCol + 5, "short   .axiscolor = %d", theAxis.axiscolor );
    SprintAt( iRow++, iCol + 5, "short     .labeled = %s",
        (theAxis.labeled) ? "TRUE" : "FALSE" );
    SprintAt( iRow++, iCol + 5, "short   .rangetype = %d", theAxis.rangetype );
    SprintAt( iRow++, iCol + 5, "float     .logbase = %f", theAxis.logbase );
    SprintAt( iRow++, iCol + 5, "short   .autoscale = %s",
        (theAxis.autoscale) ? "TRUE" : "FALSE" );
    SprintAt( iRow++, iCol + 5, "float    .scalemin = %f", theAxis.scalemin );
    SprintAt( iRow++, iCol + 5, "float    .scalemax = %f", theAxis.scalemax );
    SprintAt( iRow++, iCol + 5, "float .scalefactor = %f", theAxis.scalefactor );
    iRow = ShowTitleType( iRow, iCol + 5, theAxis.scaletitle );
    SprintAt( iRow++, iCol + 5, "float  .ticinterval = %f", theAxis.ticinterval );
    SprintAt( iRow++, iCol + 5, "short    .ticformat = %d", theAxis.ticformat );
    SprintAt( iRow++, iCol + 5, "short  .ticdecimals = %d", theAxis.ticdecimals );

    return iRow;
}

/*  ShowLegendType - Displays data in a variable of type "legendtype".
 *
 *  Params: iRow - Row at which to start
 *          iCol - Column from which to indent
 *          theLegend - Variable of type "legendtype" to display
 *
 *  Return: Next available row
 */
int ShowLegendType( int iRow, int iCol, legendtype theLegend )
{
    SprintAt( iRow++, iCol + 5, "short      .legend = %s",
              (theLegend.legend) ? "TRUE" : "FALSE" );
    SprintAt( iRow++, iCol + 5, "short       .place = %d", theLegend.place );
    SprintAt( iRow++, iCol + 5, "short   .textcolor = %d", theLegend.textcolor );
    SprintAt( iRow++, iCol + 5, "short    .autosize = %d", theLegend.autosize );
    SprintAt( iRow++, iCol + 5, "windowtype legendwindow =" );
    iRow = ShowWindowType( iRow, iCol + 5, theLegend.legendwindow );

    return iRow;
}

/*  ShowSampleData - Displays the sample data for the demo.
 *
 *  Params: None
 */
void ShowSampleData()
{
    int  iCat, y, iSeries, iValue;
    char szTmp[80];

    /* Display data in table format. */
    _clearscreen( _GCLEARSCREEN );
    PrintAt( 1, 40 - strlen(szTmp) / 2, "Data in Table Format", -1L );

    /* Write titles and draw separator line. */
    y = 3;
    for( iCat = 1; iCat <= 12; iCat++ )
    PrintAt( y, iCat * 6, aMonths[iCat - 1], -1L );

    memset( szTmp, '-', 69 );
    szTmp[69] = 0;
    PrintAt( ++y, 6, szTmp, -1L );

    /* Write data. */
    for( iSeries = 1; iSeries <= 3; iSeries++ )
    {
        PrintAt( y += 2, 4, aSalesTitles[iSeries - 1], -1L );
        y += 2;
        for( iValue = 1; iValue <= 12; iValue++ )
        {
            sprintf( szTmp, "%#3.2f", aSales[iSeries - 1][iValue - 1] );
            PrintAt( y, iValue * 6, (char far *)szTmp, -1L );
        }
    }
    PrintAt( y += 2, 4, TempTitle, -1L );
    y += 2;
    for( iValue = 1; iValue <= 12; iValue++ )
    {
        sprintf( szTmp, "%#3.1f", aTemperature[iValue - 1] );
        PrintAt( y, iValue * 6, szTmp, -1L );
    }

    PrintAt( y += 2, 1, "Press any key to continue . . .", -1L );
    getche();
}

/*  ShowTitleType - Displays data in a variable of type "titletype".
 *
 *  Params: iRow - Row at which to start
 *          iCol - Column from which to indent
 *          theTitle - Variable of type "titletype" to display
 *
 *  Return: Next available row
 */
int ShowTitleType( int iRow, int iCol, titletype theTitle )
{
    SprintAt( iRow++, iCol + 5, "char    .title[%d] = \"%s\"", _PG_TITLELEN,
                 theTitle.title );
    SprintAt( iRow++, iCol + 5, "short  .titlecolor = %d", theTitle.titlecolor );
    SprintAt( iRow++, iCol + 5, "short     .justify = %d", theTitle.justify );

    return iRow;
}

/*  ShowWindowType - Displays data in a variable of type "windowtype".
 *
 *  Params: iRow - Row at which to start
 *          iCol - Column from which to indent
 *          theWindow - Variable of type "windowtype" to display
 *
 *  Return: Next available row
 */
int ShowWindowType( int iRow, int iCol, windowtype theWindow )
{
    SprintAt( iRow++, iCol + 5, "short          .x1 = %d", theWindow.x1 );
    SprintAt( iRow++, iCol + 5, "short          .y1 = %d", theWindow.y1 );
    SprintAt( iRow++, iCol + 5, "short          .x2 = %d", theWindow.x2 );
    SprintAt( iRow++, iCol + 5, "short          .y2 = %d", theWindow.y2 );
    SprintAt( iRow++, iCol + 5, "short      .border = %d", theWindow.border );
    SprintAt( iRow++, iCol + 5, "short  .background = %d", theWindow.background );
    SprintAt( iRow++, iCol + 5, "short .borderstyle = %d", theWindow.borderstyle );
    SprintAt( iRow++, iCol + 5, "short .bordercolor = %d", theWindow.bordercolor );

    return iRow;
}

/*  ShowError - Displays error message for one of the chart library
 *  errors.
 *
 *  Params: iErr - Error number
 */
void ShowError( int iErr )
{
    char szTmp[50];

    /* Change to text screen.  */
    _setvideomode( _DEFAULTMODE );

    /* Select the error text.  */
    switch( iErr )
    {
        case _PG_NOTINITIALIZED:
            strcpy( szTmp, "Chart Library Not Initialized" );
            break;
        case _PG_BADSCREENMODE:
            strcpy( szTmp, "Invalid Screen Mode" );
            break;
        case _PG_BADCHARTTYPE:
            strcpy( szTmp, "Invalid Chart Type" );
            break;
        case _PG_BADCHARTSTYLE:
            strcpy( szTmp, "Invalid Chart Style" );
            break;
        case _PG_BADLEGENDWINDOW:
            strcpy( szTmp, "Invalid Legend Window" );
            break;
        case _PG_BADDATAWINDOW:
            strcpy( szTmp, "No Room for Data window" );
            break;
        case _PG_BADCHARTWINDOW:
            strcpy( szTmp, "Invalid Chart window coordinates" );
            break;
        case _PG_NOMEMORY:
            strcpy( szTmp, "Not Enough Memory for Data Arrays" );
            break;
        case _PG_BADLOGBASE:
            strcpy( szTmp, "X or Y log base <= 0" );
            break;
        case _PG_BADSCALEFACTOR:
            strcpy( szTmp, "X or Y scale factor = 0" );
            break;
        case _PG_TOOSMALLN:
            strcpy( szTmp, "Too few data values" );
            break;
        case _PG_TOOFEWSERIES:
            strcpy( szTmp, "No data series specified" );
            break;
        default:
            strcpy( szTmp, "Unknown error" );
    }

    ErrorMsg( szTmp );
}

/*  TitleOpt - Gets title options.
 *
 *  Params: ptt - Pointer to titletype variable
 */
void TitleOpt( titletype *ptt )
{
    int iChoice;

    PushTitle( pszTitleOpt[0] );
    do
    {
        iChoice = Menu( pszTitleOpt );
        switch( iChoice )
        {

            case 1: 
                /* Query for title text.  */
                Help( "70 characters maximum length.", co.InputColor );
                InputStr( "Enter Text: ", ptt->title );
                break;

            case 2: 
                /* Query for title color color.  */
                Help( "Enter a number in the range 0-15.", co.InputColor );
                ptt->titlecolor =
                    InputInt( "Title Color? ", ptt->titlecolor, 0, 15 );
                break;

            case 3:
                /* Get justify option.  */
                Justify( ptt );
        }
        ClrHelp();

    } while( iChoice != ESCAPE );
    PopTitle();
}

/*  Titles - Manages Main and Sub title menus.
 *
 *  Params: None
 */
void Titles()
{
    int iChoice;

    PushTitle( pszTitles[0] );
    do
    {
        iChoice = Menu( pszTitles );
        switch( iChoice )
        {

            case 1:
                /* Fix menu title and get options for main title.  */
                pszTitleOpt[0] = "MainTitle";
                TitleOpt( &ce.maintitle );
                break;

            case 2:
                /* Fix menu title and get options for subtitle.  */
                pszTitleOpt[0] = "Sub Title";
                TitleOpt( &ce.subtitle );
        }
    } while( iChoice != ESCAPE );
    PopTitle();
}

/*  ViewChart - Draws the current chart.
 *
 *  Params: None
 */
int ViewChart()
{
    int cValue, iValue, iSeries, iErr;

    /* Make sure some data exists.  */
    if( cSeries <= 0 )
    {
        fDefault = TRUE;
        DefaultData( ce.charttype, ce.chartstyle, FALSE );
    }

    /* Find the longest series.  */
    cValue = 0;
    for( iSeries = 0; iSeries < cSeries; iSeries++ )
        if( acSeries[iSeries] > cValue )
            cValue = acSeries[iSeries];

    _setvideomode( si.mode );


    /* Process depending on the type of chart.  */
    switch( ce.charttype )
    {

        case _PG_PIECHART:
        case _PG_BARCHART:
        case _PG_COLUMNCHART:
        case _PG_LINECHART:

            /* Initialize data and draw pie chart or single-series bar,
             * column, or line chart.
             */
            if( (cSeries == 1) ||( ce.charttype == _PG_PIECHART) )
            {

                /* Transfer data into a single-dimension array.  */
                for( iValue = 0; iValue < cValue; iValue++ )
                   axValue[iValue] = aValue[0][iValue];

                /* Draw chart.  */
                if( ce.charttype == _PG_PIECHART )
                    iErr = _pg_chartpie( &ce, aCat, axValue,
                                         aExplode, cValue );
                else
                    iErr = _pg_chart( &ce, aCat, axValue, cValue );
            }
            /* If multiple-series, then data is OK. Just draw chart.  */
            else
                iErr = _pg_chartms( &ce, aCat, (float far *)aValue,
                                    cSeries, cValue, cValue, aSeriesName );
            break;

        case _PG_SCATTERCHART:

            /* Make sure there are enough data sets.  */
            if( cSeries == 1 )
            {
                _setvideomode( _DEFAULTMODE );
                si.help = 10;
                ErrorMsg( "Too few value data columns for scatter chart." );
                return 1;

            }
            /* If it's a single-series scatter, transfer data to one-
             * dimensional arrays and make chart call.
             */
            else if( cSeries == 2 )
            {
                for( iValue = 0; iValue < cValue; iValue++ )
                {
                    axValue[iValue] = aValue[0][iValue];
                    ayValue[iValue] = aValue[1][iValue];
                }
                cSeries = 1;
                iErr = _pg_chartscatter( &ce, axValue, ayValue, cValue );

            }
            /* If it's a multiple-series scatter, transfer odd columns to
             * X-axis data array and even columns to Y-axis array and make
             * chart call.
             */
            else
            {

                for( iSeries = 1; iSeries < cSeries; iSeries += 2 )
                {
                    aSeriesName[iSeries / 2] = aSeriesName[iSeries - 1];
                    for( iValue = 0; iValue < cValue; iValue++ )
                    {
                        axValueMS[iSeries / 2][iValue] =
                            aValue[iSeries - 1][iValue];
                        ayValueMS[iSeries / 2][iValue] =
                            aValue[iSeries][iValue];
                    }
                }
                cSeries /= 2;

                iErr = _pg_chartscatterms( &ce, (float far *)axValueMS,
                                           (float far *)ayValueMS,
                                           cSeries, cValue, cValue,
                                           aSeriesName );
            }
    }

    if( !fDefault )
        ClearData( FALSE );

    /* If error, show it, else wait for keypress with chart on screen.  */
    if( iErr )
    {
        ShowError( iErr );
        return iErr;
    }
    else
        return ( getch() == ESCAPE );   /* ESCAPE means stop demo */
}

/*  Windows - Selects chart or data window, and gets options for either.
 *
 *  Params: None
 */
void Windows()
{
    int iChoice;

    PushTitle( pszWindows[0] );
    do
    {

        /* Select window and get options for it.  */
        iChoice = Menu( pszWindows );
        switch( iChoice )
        {

            case 1:
                ChartWindow();
                break;

            case 2:
                DataWindow();

        }
    } while( iChoice != ESCAPE );
    PopTitle();
}

/*  WindowSize - Gets coordinates for window location and size.
 *
 *  Params: pwt - pointer to windowtype variable
 */
void WindowSize( windowtype *pwt )
{
    int iChoice;

    /* Get window size settings.  */
    PushTitle( pszSize[0] );
    do
    {
        /* Query for top, bottom, left, or right of window.  */
        iChoice = Menu( pszSize );
        switch( iChoice )
        {

            case 1:
                Help( "Enter window top in pixels.", co.InputColor );
                pwt->y1 = InputInt( "Top? ", pwt->y1, 0, si.yMax );
                break;

            case 2:
                Help( "Enter window Left in pixels.", co.InputColor );
                pwt->x1 = InputInt( "Left? ", pwt->x1, 0, si.xMax );
                break;

            case 3:
                Help( "Enter window bottom in pixels.", co.InputColor );
                pwt->y2 = InputInt( "Bottom? ", pwt->y2, 0, si.yMax );
                break;

            case 4:
                Help( "Enter window right in pixels.", co.InputColor );
                pwt->x2 = InputInt( "Right? ", pwt->x2, 0, si.xMax );
        }
    } while( iChoice != ESCAPE );
    PopTitle();
}
