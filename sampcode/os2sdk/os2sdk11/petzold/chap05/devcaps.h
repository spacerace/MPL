/*-----------------------
   DEVCAPS.H header file
  -----------------------*/

#define NUMLINES (sizeof devcaps / sizeof devcaps [0])

struct
     {
     LONG lIndex ;
     CHAR *szIdentifier ;
     }
     devcaps [] =
     {
     CAPS_FAMILY                 , "CAPS_FAMILY"                 ,
     CAPS_IO_CAPS                , "CAPS_IO_CAPS"                ,
     CAPS_TECHNOLOGY             , "CAPS_TECHNOLOGY"             ,
     CAPS_DRIVER_VERSION         , "CAPS_DRIVER_VERSION"         ,
     CAPS_HEIGHT                 , "CAPS_HEIGHT"                 ,
     CAPS_WIDTH                  , "CAPS_WIDTH"                  ,
     CAPS_HEIGHT_IN_CHARS        , "CAPS_HEIGHT_IN_CHARS"        ,
     CAPS_WIDTH_IN_CHARS         , "CAPS_WIDTH_IN_CHARS"         ,
     CAPS_VERTICAL_RESOLUTION    , "CAPS_VERTICAL_RESOLUTION"    ,
     CAPS_HORIZONTAL_RESOLUTION  , "CAPS_HORIZONTAL_RESOLUTION"  ,
     CAPS_CHAR_HEIGHT            , "CAPS_CHAR_HEIGHT"            ,
     CAPS_CHAR_WIDTH             , "CAPS_CHAR_WIDTH"             ,
     CAPS_SMALL_CHAR_HEIGHT      , "CAPS_SMALL_CHAR_HEIGHT"      ,
     CAPS_SMALL_CHAR_WIDTH       , "CAPS_SMALL_CHAR_WIDTH"       ,
     CAPS_COLORS                 , "CAPS_COLORS"                 ,
     CAPS_COLOR_PLANES           , "CAPS_COLOR_PLANES"           ,
     CAPS_COLOR_BITCOUNT         , "CAPS_COLOR_BITCOUNT"         ,
     CAPS_COLOR_TABLE_SUPPORT    , "CAPS_COLOR_TABLE_SUPPORT"    ,
     CAPS_MOUSE_BUTTONS          , "CAPS_MOUSE_BUTTONS"          ,
     CAPS_FOREGROUND_MIX_SUPPORT , "CAPS_FOREGROUND_MIX_SUPPORT" ,
     CAPS_BACKGROUND_MIX_SUPPORT , "CAPS_BACKGROUND_MIX_SUPPORT" ,
     CAPS_VIO_LOADABLE_FONTS     , "CAPS_VIO_LOADABLE_FONTS"     ,
     CAPS_WINDOW_BYTE_ALIGNMENT  , "CAPS_WINDOW_BYTE_ALIGNMENT"  ,
     CAPS_BITMAP_FORMATS         , "CAPS_BITMAP_FORMATS"         ,
     CAPS_RASTER_CAPS            , "CAPS_RASTER_CAPS"            ,
     CAPS_MARKER_HEIGHT          , "CAPS_MARKER_HEIGHT"          ,
     CAPS_MARKER_WIDTH           , "CAPS_MARKER_WIDTH"           ,
     CAPS_DEVICE_FONTS           , "CAPS_DEVICE_FONTS"           ,
     CAPS_GRAPHICS_SUBSET        , "CAPS_GRAPHICS_SUBSET"        ,
     CAPS_GRAPHICS_VERSION       , "CAPS_GRAPHICS_VERSION"       ,
     CAPS_GRAPHICS_VECTOR_SUBSET , "CAPS_GRAPHICS_VECTOR_SUBSET" ,
     CAPS_DEVICE_WINDOWING       , "CAPS_DEVICE_WINDOWING"       ,
     CAPS_ADDITIONAL_GRAPHICS    , "CAPS_ADDITIONAL_GRAPHICS"    ,
     CAPS_PHYS_COLORS            , "CAPS_PHYS_COLORS"            ,
     CAPS_COLOR_INDEX            , "CAPS_COLOR_INDEX"            ,
     CAPS_GRAPHICS_CHAR_WIDTH    , "CAPS_GRAPHICS_CHAR_WIDTH"    ,
     CAPS_GRAPHICS_CHAR_HEIGHT   , "CAPS_GRAPHICS_CHAR_HEIGHT"   ,
     CAPS_HORIZONTAL_FONT_RES    , "CAPS_HORIZONTAL_FONT_RES"    ,
     CAPS_VERTICAL_FONT_RES      , "CAPS_VERTICAL_FONT_RES"
     } ;
