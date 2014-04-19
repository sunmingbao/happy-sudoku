/* 
 * �����Ϊ��ѡ���Դ�����
 * ������İ�Ȩ(����Դ�뼰�����Ʒ����汾)��һ�й������С�
 * ����������ʹ�á������������
 * ��Ҳ�������κ���ʽ���κ�Ŀ��ʹ�ñ����(����Դ�뼰�����Ʒ����汾)���������κΰ�Ȩ���ơ�
 * =====================
 * ����: ������
 * ����: sunmingbao@126.com
 */


#ifndef __GLOBAL_SYMBOLS_H_
#define __GLOBAL_SYMBOLS_H_

#include <windows.h>
#include "defs.h"



extern HINSTANCE g_hInstance;
extern int init_over;

extern int cxChar; /* SYSTEM_FIXED_FONT width */
extern int cyChar; /* SYSTEM_FIXED_FONT height */
extern int scrn_width;  /* ��Ļ��� */
extern int scrn_height; /* ��Ļ�߶� */

#define    TIMER_GAME_USE_TIME_CNT    (1)

#define TIMER_TIP_WIN_SHOW    2
#define TIMER_TIP_WIN_SHOW_GAP 3

#define TIMER_TIP_WIN_HIDE    3
#define TIMER_TIP_WIN_HIDE_GAP 30

#define TIMER_TIP_WIN_LAST    4
#define TIMER_TIP_WIN_LAST_GAP 500

#define TIMER_GEN_PUZZLE_WAIT    5
#define TIMER_GEN_PUZZLE_WAIT_GAP 500

int register_frame();
void set_frame_title(TCHAR *file_name);

BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT message,WPARAM wParam, LPARAM lParam);

extern HWND    hwnd_frame;
extern HWND hwndTip;
extern int  doc_modified;
void save_doc_file(TCHAR *file_name);
void load_doc_file(TCHAR *file_name);


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

int register_main_board_win();
int stage_mode_begin();
void select_stage(int stage_idx);
void select_empty_game();
void LoadRandGame();
void SaveAsArch(char *file_path);
void SaveAsPuzzle(char *file_path);
void load_puzzle_as_game(char *file_path);
void LoadArch(char *file_path);
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
int register_input_grid_win();
void display_input_win(int row, int col);
void display_mark_win(int row, int col);
void mov_input_win_to_grid(int row, int col);
extern TCHAR szInputBoardWinClassName[] ;
extern HWND    hwnd_input_board;

extern TCHAR szTipWinClassName[];
extern HWND    hwnd_tip;
int register_tip_win();
void show_tip(TCHAR *info);

void get_puzzle_and_resolve();
int generate_puzzle(char *output);
#endif

