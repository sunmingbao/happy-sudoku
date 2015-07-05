/* 
 * 本软件为免费、开源软件。
 * 本软件的版权(包括源码及二进制发布版本)归一切公众所有。
 * 您可以自由使用、传播本软件。
 * 您也可以以任何形式、任何目的使用本软件(包括源码及二进制发布版本)，而不受任何版权限制。
 * =====================
 * 作者: 孙明保
 * 邮箱: sunmingbao@126.com
 */


#ifndef __GLOBAL_SYMBOLS_H_
#define __GLOBAL_SYMBOLS_H_

#include <windows.h>
#include "defs.h"



extern HINSTANCE g_hInstance;
extern int init_over;

extern int cxChar; /* SYSTEM_FIXED_FONT width */
extern int cyChar; /* SYSTEM_FIXED_FONT height */
extern int scrn_width;  /* 屏幕宽度 */
extern int scrn_height; /* 屏幕高度 */

#define    TIMER_GAME_USE_TIME_CNT    (1)

#define TIMER_TIP_WIN_SHOW    2
#define TIMER_TIP_WIN_SHOW_GAP 3

#define TIMER_TIP_WIN_HIDE    3
#define TIMER_TIP_WIN_HIDE_GAP 30

#define TIMER_TIP_WIN_LAST    4
#define TIMER_TIP_WIN_LAST_GAP 500

#define TIMER_GEN_PUZZLE_WAIT    5
#define TIMER_GEN_PUZZLE_WAIT_GAP 500

#define  TIMER_VER_UPDATE_1   5
#define  TIMER_VER_UPDATE_1_GAP  50

#define  TIMER_VER_UPDATE_2   6
#define  TIMER_VER_UPDATE_2_GAP  1000

#define    APP_PROFILE_FILE         ".\\profile.ini"
#define    VER_UPDATE_NOTICE_RCD    ".\\ver_updt_notice_rcd.ini"

int register_frame();
void set_frame_title(TCHAR *file_name);

BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT message,WPARAM wParam, LPARAM lParam);

extern HWND    hwnd_frame;
extern HWND hwndTip;
extern int  doc_modified;
void save_doc_file(TCHAR *file_name);
void load_doc_file(TCHAR *file_name);

DWORD WINAPI  ver_update(LPVOID lpParameter);

extern HWND    hwnd_toolbar;
extern int     toolbar_height;

extern HWND    hwnd_statusbar;
extern int     statusbar_height;


int CreateToolbar();
void set_toolbar_button(int id, int is_enabled);
int CreateStatusBar();
void update_statusbar();
int create_windows(int iCmdShow);

extern TCHAR szMainBoardWinClassName[];
extern HWND    hwnd_main_board;
extern char  new_version_notice[8];
int register_main_board_win();
int stage_mode_begin();
void select_stage(int stage_idx);
void select_empty_game();
void LoadRandGame();
void SaveAsArch(char *file_path);
void SaveAsPuzzle(char *file_path);
int puzzle_file_to_input_str(HWND hparent, char *output, char *file_path);
void load_puzzle_as_game(char *file_path);
void LoadArch(char *file_path);
void input_str_to_hm_str(char * output, char *input);
void game_to_hm_str(char * output);
void refresh_grid_on_focus();
extern int display_help;
extern int display_time;
extern int play_music;
int register_grid_win();
void unfocus_any_grid();
void * grid_mark_str(int row, int col);
void un_do();
void re_do();
int can_un_do();
int can_re_do();
void update_statusbar_time();
void resize_window(HWND hwnd);

int register_input_board_win();
int register_mark_win();
int register_input_grid_win();
void display_input_win(int row, int col);
void display_mark_win(int row, int col);
void mov_input_mark_win_to_grid(int row, int col);
void hide_input_mark_win();
void refresh_input_mark_win();
extern int cur_row, cur_col;
typedef struct
{
    HWND hwnd;
    int  row;
    int  col;
    int  input_by_user;
    char mark[9+2];
    HBRUSH hBrush;
}t_grid_info;

extern t_grid_info at_grid[9][9];

#define    COLOUR_NO_VALUE         RGB(0x55,0xaa,0x55)
#define    COLOUR_HAS_VALUE        RGB(0xdd,0x92,0x22)
#define    COLOUR_INPUT_BY_USER    RGB(0xcc,0x55,0xcc)
#define    COLOUR_ON_FOCUS         RGB(0x99,0x00,0x00)

#define    COLOUR_BLINK_1          RGB(0x00,0x00,0x00)
#define    COLOUR_BLINK_2          RGB(0xff,0xff,0xff)

#define    COLOR_MARK_DIGIT        RGB(0xff,0xff,0xff)

extern TCHAR szInputBoardWinClassName[] ;
extern TCHAR szMarkBoardWinClassName[] ;
extern HWND    hwnd_input_board;
extern HWND    hwnd_mark_board;

extern TCHAR szTipWinClassName[];
extern HWND    hwnd_tip;
int register_tip_win();
void show_tip(TCHAR *info);

void get_puzzle_and_resolve(char *input);
int generate_puzzle(char *output);

int select_int_value(char *CAP, char *tip, int min, int max, int *presult);

#define    GAP_SIZE          (3)
#define    GAP_SIZE_BLOCK    (10)
#define    GAP_SIZE_TOTAL    (GAP_SIZE*8 + GAP_SIZE_BLOCK*2)
#endif

