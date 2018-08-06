#include <conio.h>
#include <graph.h>
#include <dos.h>
#include <memory.h>
#include <malloc.h>

#define BLACK        0x00
#define BLUE         0x01
#define GREEN        0x02
#define CYAN         0x03
#define RED          0x04
#define MAGENTA      0x05
#define BROWN        0x06
#define LIGHTGRAY    0x07
#define DARKGRAY     0x08
#define LIGHTBLUE    0x09
#define LIGHTGREEN   0x0A
#define LIGHTCYAN    0x0B
#define LIGHTRED     0x0C
#define LIGHTMAGENTA 0x0D
#define YELLOW       0x0E
#define WHITE        0x0F
#define UP_LEFT_CORNER         0xC9
#define UP_RIGHT_CORNER        0xBB
#define DOWN_LEFT_CORNER       0xC8
#define DOWN_RIGHT_CORNER      0xBC
#define HORZ_LINE              0xCD
#define VERT_LINE              0xBA
#define MAX_WINDOW_NUMBER      16

struct window_boundaries
  {
  int current_window;
  char far *hold_address[MAX_WINDOW_NUMBER];
  short cursor_row[MAX_WINDOW_NUMBER];
  short cursor_col[MAX_WINDOW_NUMBER];
  short fore_color[MAX_WINDOW_NUMBER];
  short back_color[MAX_WINDOW_NUMBER];
  short up_row[MAX_WINDOW_NUMBER];
  short up_col[MAX_WINDOW_NUMBER];
  short down_row[MAX_WINDOW_NUMBER];
  short down_col[MAX_WINDOW_NUMBER];
  } window;

  int makewindow();
  int unmakewindow();
  void set_video_mode();
  int video_mode_set;
  int video_base;
