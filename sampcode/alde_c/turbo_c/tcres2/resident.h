/*  Resident include file                   *
 *  (C) KyCorp Information Group, Inc. 1987 */

#define NULL					0
#define KEY_ESC            1
#define KEY_1              2
#define KEY_2              3
#define KEY_3              4
#define KEY_4              5
#define KEY_5              6
#define KEY_6              7
#define KEY_7              8
#define KEY_8              9
#define KEY_9              10
#define KEY_0              11
#define KEY__              12
#define KEY_EQUALS         13
#define KEY_BACK_SPACE     14
#define KEY_TAB            15
#define KEY_Q              16
#define KEY_W              17
#define KEY_E              18
#define KEY_R              19
#define KEY_T              20
#define KEY_Y              21
#define KEY_U              22
#define KEY_I              23
#define KEY_O              24
#define KEY_P              25
#define KEY_OPEN_BRACKET   26
#define KEY_CLOSE_BRACKET  27
#define KEY_ENTER          28
#define KEY_CTRL           29
#define KEY_A              30
#define KEY_S              31
#define KEY_D              32
#define KEY_F              33
#define KEY_G              34
#define KEY_H              35
#define KEY_J              36
#define KEY_K              37
#define KEY_L              38
#define KEY_COLON          39
#define KEY_QUOTE          40
#define KEY_TILDE          41
#define KEY_LEFT_SHIFT     42
#define KEY_BACK_SLACH     43  /* back slash */
#define KEY_Z              44
#define KEY_X              45
#define KEY_C              46
#define KEY_V              47
#define KEY_B              48
#define KEY_N              49
#define KEY_M              50
#define KEY_COMMA          51
#define KEY_PERIOD         52
#define KEY_FORWARD_SLASH  53
#define KEY_RIGHT_SHIFT    54
#define KEY_ASTERISKS      55  /* The keypad "*" key */
#define KEY_ALT            56
#define KEY_SPACE          57
#define KEY_CAPS_LOCK      58
#define KEY_F1             59
#define KEY_F2             60
#define KEY_F3             61
#define KEY_F4             62
#define KEY_F5             63
#define KEY_F6             64
#define KEY_F7             65
#define KEY_F8             66
#define KEY_F9             67
#define KEY_F10            68
#define KEY_NUM_LOCK       69
#define KEY_SCROLL_LOCK    70
#define KEY_HOME           71
#define KEY_UP             72
#define KEY_PGUP           73
#define KEY_MINUS          74  /* The keypad minus key */
#define KEY_LEFT           75
#define KEY_FIVE           76  /* The "5" key on the keypad */
#define KEY_RIGHT          77
#define KEY_PLUS           78  /* The keypad plus key */
#define KEY_END            79
#define KEY_DOWN           80
#define KEY_PGDN           81
#define KEY_INS            82
#define KEY_DEL            83
      
#define BLACK        0
#define BLUE         1
#define GREEN        2
#define CYAN         3
#define RED          4
#define MAGENTA      5
#define BROWN        6
#define GRAY         7
#define LT_BLUE      9
#define LT_GREEN     10
#define LT_CYAN      11
#define LT_RED       12
#define LT_MAGENTA   13
#define YELLOW       14
#define WHITE        15
#define MONOCHROME   2

int   blinking (int);
void  border (int);
void  box (int, int, int, int, int, int);
int   bright (int);
int   chk_video (void);
int   color (int, int);
void  cursize (int, int);
void  dis_cur (int, int, int, int);
void  dis_str (int, int, char *, int);
int	drop_tsr (void);
void  erase (int, int, int, int);
void  exch_window (struct WINDOW *);
int   getcur (void);
int   go_resident (void (*)(), int, int, int);
void  hidecur (void);
void  hilight (int, int, int, int);
void  hor_line (int, int, int, int, int);
int   iscolor (void);
void  keystrokes (char *, int *);
void  key_window (int, struct WINDOW *);
int   loaded (void);
void  make_window (struct WINDOW *);
void  move_window (struct WINDOW *, int, int);
void  normalcur (void);
void  save_window (struct WINDOW *);

#define SCREEN_DIRECT   0xff
#define SCREEN_FAST     1
#define SCREEN_SLOW     8
int SCREEN_SPEED = SCREEN_SLOW;

void  setcur (int);

unsigned int STACK_SIZE = 2000;

int	top_tsr (void);
int   underlined (int);
void  ver_line (int, int, int, int, int);

#define NO_LINES        0
#define SINGLE_LINE     1
#define DOUBLE_LINE     2
struct WINDOW
   {
      unsigned int row;
      unsigned int col;
      unsigned int hlen;
      unsigned int vlen;
      unsigned int color;
      int *buffer;    /* size should be = ((vlen + 2) * (hlen + 2)) */
      unsigned int vtype;
      unsigned int htype;
		unsigned int cur_row;
		unsigned int cur_col;
		unsigned int text_color;
   };

struct WINDOW error_w;

