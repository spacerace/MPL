#include <graph.h>
#include <windows.h>


void set_text_mode()
  {
  if (_setvideomode(_TEXTC80))
    {
    video_mode_set = _TEXTC80;
    video_base = 0xb800;
    }
  else
    if (_setvideomode(_TEXTBW80))
      {
      video_mode_set = _TEXTBW80;
      video_base = 0xb000;
      }
    else
      if (_setvideomode(_TEXTC40))
        {
        video_mode_set = _TEXTC40;
        video_base = 0xb800;
        }
      else
        if (_setvideomode(_TEXTBW40))
          {
          video_mode_set = _TEXTBW40;
          video_base = 0xb000;
          }
        else
          if (_setvideomode(_TEXTMONO))
            {
            video_mode_set = (_TEXTMONO);
            video_base = 0xb000;
            }
  }

int makebxwindow(up_row, up_col, down_row, down_col, fore_color, back_color,
  border_fore, border_back)
  short up_row;
  short up_col;
  short down_row;
  short down_col;
  short fore_color;
  short back_color;
  short border_fore;
  short border_back;
  {
  int window_made = 1;
  int count;
  int max_count;
  short old_wrap;
  char hold_char[2];

  if (((down_row - up_row) < 3) || ((down_col - up_col) < 5))
    return(0);
  old_wrap = _wrapon(_GWRAPOFF);
  window_made = makewindow(up_row,up_col,down_row,down_col,
  border_fore,border_back);
  if (!window_made)
    {
    _wrapon(old_wrap);
    return(0);
    }
  hold_char[0] = UP_LEFT_CORNER;
  hold_char[1] = '\00';
  _outtext(hold_char);
  hold_char[0] = HORZ_LINE;
  max_count = (down_col - up_col) - 1;
  for (count = 0; count < max_count; count++)
    _outtext(hold_char);
  hold_char[0] = UP_RIGHT_CORNER;
  _outtext(hold_char);
  hold_char[0] = VERT_LINE;
  max_count = (down_row - up_row) + 1;
  for (count = 2; count < max_count; count++)
    {
    _settextposition(count, down_col);
    _outtext(hold_char);
    }
  _settextposition(down_row,1);
  hold_char[0] = DOWN_LEFT_CORNER;
  _outtext(hold_char);
  hold_char[0] = HORZ_LINE;
  max_count = (down_col - up_col) - 1;
  for (count = 0; count < max_count; count++)
    _outtext(hold_char);
  hold_char[0] = DOWN_RIGHT_CORNER;
  _outtext(hold_char);
  hold_char[0] = VERT_LINE;
  max_count = (down_row - up_row) + 1;
  for (count = 2; count < max_count; count++)
    {
    _settextposition(count, 1);
    _outtext(hold_char);
    }
  hold_char[0] = VERT_LINE;
  up_row += 1;
  up_col += 2;
  down_row -= 1;
  down_col -= 2;
  window_made = makewindow(up_row,up_col,down_row,down_col,
    fore_color,back_color);
  _wrapon(old_wrap);
  if (!window_made)
    return(0);
  else
    return(1);
  }


int makewindow(up_row, up_col, down_row, down_col, fore_color, back_color)
  short up_row;
  short up_col;
  short down_row;
  short down_col;
  short fore_color;
  short back_color;
  {
  int num_bytes;
  int bytes_per_line;
  int loop;
  int start_pos;
  unsigned int vid_offset = 0;
  struct rccoord cp;
  struct videoconfig vc;

  if (window.current_window > (MAX_WINDOW_NUMBER - 1))
    return(0);
  _getvideoconfig(&vc);
  if ((down_row <= up_row) || (down_col <= up_col))
    return(0);
  if ((up_row < 1) || (up_row > vc.numtextrows))
    return(0);
  if ((up_col < 1) || (up_col > vc.numtextcols))
    return(0);
  if ((down_row < 1) || (down_row > vc.numtextrows))
    return(0);
  if ((down_col < 1) || (down_col > vc.numtextcols))
    return(0);
  if ((video_mode_set == _TEXTBW40) || (video_mode_set == _TEXTBW80) ||
    (video_mode_set == _TEXTMONO))
    {
    fore_color = WHITE;
    back_color = BLACK;
    }
  if (window.current_window == 0)
    {
    window.up_row[0] = 1;
    window.up_col[0] = 1;
    window.down_row[0] = vc.numtextrows;
    window.down_col[0] = vc.numtextcols;
    num_bytes = ((vc.numtextrows * vc.numtextcols) * 2);
    window.hold_address[0] = _fmalloc(num_bytes);
    movedata(video_base, 0x0000, FP_SEG(window.hold_address[0]),
      FP_OFF(window.hold_address[0]), num_bytes);
    }
  cp = _gettextposition();
  window.cursor_row[window.current_window] = cp.row;
  window.cursor_col[window.current_window] = cp.col;
  window.fore_color[window.current_window] = _gettextcolor();
  window.back_color[window.current_window] = _getbkcolor();
  window.current_window += 1;
  _settextcolor(fore_color);
  _setbkcolor(back_color);
  num_bytes = (((down_row - up_row) + 1) * ((down_col - up_col) + 1)) * 2;
  window.hold_address[window.current_window] = _fmalloc(num_bytes);
  bytes_per_line = ((down_col - up_col) + 1) * 2;
  for (loop = up_row; loop < (down_row + 1); loop++)
    {
    start_pos = (((loop - 1) * (vc.numtextcols * 2)) + ((up_col - 1) * 2));
    movedata(video_base, start_pos,
      FP_SEG(window.hold_address[window.current_window]),
      (FP_OFF(window.hold_address[window.current_window]) + vid_offset),
      bytes_per_line);
    vid_offset += bytes_per_line;
    }
  _settextwindow(up_row, up_col, down_row, down_col);
  _clearscreen(_GWINDOW);
  window.up_row[window.current_window] = up_row;
  window.up_col[window.current_window] = up_col;
  window.down_row[window.current_window] = down_row;
  window.down_col[window.current_window] = down_col;
  return(1);
  }

int unmakebxwindow()
  {
  unmakewindow();
  unmakewindow();
  }

int unmakewindow()
  {
  int loop;
  unsigned int vid_offset = 0;
  int start_pos;
  int bytes_per_line;
  struct rccoord cp;
  struct videoconfig vc;

  if (window.current_window == 0)
    return(0);
  _getvideoconfig(&vc);
  bytes_per_line = ((window.down_col[window.current_window] -
    window.up_col[window.current_window]) + 1) * 2;
  for (loop = window.up_row[window.current_window];
    loop < (window.down_row[window.current_window] + 1); loop++)
    {
    start_pos = (((loop - 1) * (vc.numtextcols * 2)) +
      ((window.up_col[window.current_window] - 1) * 2));
    movedata(FP_SEG(window.hold_address[window.current_window]),
    (FP_OFF(window.hold_address[window.current_window]) + vid_offset),
    video_base, start_pos, bytes_per_line);
    vid_offset += bytes_per_line;
    }
  window.current_window -= 1;
  _settextcolor(window.fore_color[window.current_window]);
  _setbkcolor(window.back_color[window.current_window]);
  _settextwindow(window.up_row[window.current_window],
    window.up_col[window.current_window],
    window.down_row[window.current_window],
    window.down_col[window.current_window]);
  _settextposition(window.cursor_row[window.current_window],
    window.cursor_col[window.current_window]);
  return(1);
  }

