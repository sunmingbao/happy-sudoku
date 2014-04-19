/* 
 * 本软件为免费、开源软件。
 * 本软件的版权(包括源码及二进制发布版本)归一切公众所有。
 * 您可以自由使用、传播本软件。
 * 您也可以以任何形式、任何目的使用本软件(包括源码及二进制发布版本)，而不受任何版权限制。
 * =====================
 * 作者: 孙明保
 * 邮箱: sunmingbao@126.com
 */
#include <windows.h>
#include <stdlib.h>
#include "global_symbols.h"
#include "app_property.h"
#include "res.h"

#include "common.h"
#include "utils.h"
#include "file_open_history.h"
#include "ext_utils_textout.h"
#include "sudoku.h"
#include "list.h"

TCHAR szMainBoardWinClassName[] = TEXT ("main_board") ;
HWND    hwnd_main_board;
int display_help = 1;
int display_time = 1;
int play_music = 1;

#define    GAP_SIZE    (5)
static int win_size, grid_size, big_font_size, small_font_size;

#define    COLOUR_NO_VALUE         RGB(0x55,0xaa,0x55)
#define    COLOUR_HAS_VALUE        RGB(0xdd,0x92,0x22)
#define    COLOUR_INPUT_BY_USER    RGB(0xcc,0x55,0xcc)
#define    COLOUR_ON_FOCUS         RGB(0x99,0x00,0x00)

#define    COLOUR_BLINK_1          RGB(0x00,0x00,0x00)
#define    COLOUR_BLINK_2          RGB(0xff,0xff,0xff)

HBRUSH hBrush_NO_VALUE, hBrush_HAS_VALUE, hBrush_INPUT_BY_USER, hBrush_ON_FOCUS;
HBRUSH hBrush_BLINK_1, hBrush_BLINK_2;

#define    DAY_SEC    (60*60*24)
#define    HOUR_SEC   (60*60)
#define    MIN_SEC   (60)

int        game_use_time, game_left_time = 7*DAY_SEC;
t_board gt_board, gt_board_tmp;
t_board *pt_board = &gt_board;

int cur_row, cur_col;

typedef struct
{
    int num;
    t_board  at_steps[MAX_GRID_NUM];
    t_pos at_pos[MAX_GRID_NUM];
    char  ac_value[MAX_GRID_NUM];
} t_roll_back_q;
t_roll_back_q gt_rb_q;

typedef struct
{
    HWND hwnd;
    int  row;
    int  col;
    int  input_by_user;
    char mark[MAX_DIGIT_NUM+2];
    HBRUSH hBrush;
}t_grid_info;

t_grid_info at_grid[9][9];

void *get_grid_info(HWND hwnd)
{
    int i, j;
    for (i=0;i<9;i++)
        for (j=0;j<9;j++)
    {
        if (at_grid[i][j].hwnd == hwnd)
            return &(at_grid[i][j]);

    }


    return NULL;
}

char empty_stage[] =
"........."
"........."
"........."
"........."
"........."
"........."
"........."
"........."
".........";

char cur_stage[82];
char tmp_stage[82];
enum
{
    StageMode = 1000,
    FreeMode,
    UsrLoadMode,
    EmptyMode,
    RandomMode,
};

const char *mode_names[] = 
{
"闯关模式", 
"选关模式",
"自加载迷题",
"空棋盘模式",
"随机模式",
};

const char * mode_str(int mode)
{
    return mode_names[mode - StageMode];
}

int nr_stages;
int cur_stage_idx=1;
int game_mode = StageMode;
int game_over = 0;

void InitStageNum()
{
    char buf[128]={0};

    FILE *fp=fopen("stages.txt","r");
    if (NULL==fp) return;

    fgets(buf, sizeof(buf), fp);
    sscanf(buf, "stage_num=%d \r\n", &nr_stages);
    fclose(fp);

}

void load_stage(char *output, int idx)
{
    char buf[128]={0};
    int cnt = 0;
    char *pchar;

    FILE *fp=fopen("stages.txt","r");
    if (NULL==fp) return;

    while (buf[0]!='[' || atoi(buf+1)!=idx)
    {
        fgets(buf, sizeof(buf), fp);
    }

    while (cnt<81)
    {
        fgets(buf, sizeof(buf), fp);
        pchar = buf;
        while ((*pchar)!=0)
        {
            if ((*pchar) == '.' || ((*pchar) >= '1' && (*pchar) <= '9'))
            {
                output[cnt] = *pchar;
                cnt++;
            }
            pchar++;
        }
    }

    output[cnt] = 0;
    fclose(fp);

}

void time_str(char *info, int sec_value)
{
    int day = 0, hour=0, min=0, sec=0;

    if (sec_value>=DAY_SEC)
    {
        day = sec_value/DAY_SEC;
        sec_value %= DAY_SEC;
    }

    if (sec_value>=HOUR_SEC)
    {
        hour = sec_value/HOUR_SEC;
        sec_value %= HOUR_SEC;
    }

    if (sec_value>=MIN_SEC)
    {
        min = sec_value/MIN_SEC;
        sec_value %= MIN_SEC;
    }

    sec = sec_value;

    if (day)
        sprintf(info, "%d天%d时%d分%d秒", day, hour, min, sec);
    else if (hour)
        sprintf(info, "%d时%d分%d秒", hour, min, sec);
    else if (min)
        sprintf(info, "%d分%d秒", min, sec);
    else 
        sprintf(info, "%d秒", sec);

}


static inline void init_rb_q(t_roll_back_q *pt_rb_q)
{
    pt_rb_q->num = 0;
    memset(pt_rb_q->ac_value
        , NO_VALUE_CHAR
        , sizeof(pt_rb_q->ac_value));
}

void refresh_board()
{
    int i, j;
    for (i=0; i<MAX_ROW_NUM; i++)
        for (j=0; j<MAX_COL_NUM; j++)
            refresh_grid(i, j);
}

void InitNewGame(const char *input)
{
    int i;
    int row, col;

cur_row = cur_col = -1;
pt_board = &gt_board;
load_game(pt_board, input);

    init_rb_q(&gt_rb_q);

    for (i=0;i<MAX_GRID_NUM; i++)
    {
        row = i/MAX_COL_NUM;
        col = i%MAX_COL_NUM;

        if (!isdigit(input[i]))
            at_grid[row][col].input_by_user = 1;
        else
            at_grid[row][col].input_by_user = 0;

        strcpy(at_grid[row][col].mark, "123456789 ");
    }

}

void InitGrids()
{

    int row, col;

    for (row=0; row<MAX_ROW_NUM; row++)
        for (col=0; col<MAX_COL_NUM; col++)
    {

        at_grid[row][col].row = row;
        at_grid[row][col].col = col;
        at_grid[row][col].hBrush = NULL;
    }

}

void update_statusbar()
{
    
    char info[128];
    if (StageMode==game_mode)
    sprintf(info, "%s: %d关/%d关"
        , mode_str(game_mode)
        ,cur_stage_idx
        ,nr_stages);
    else if (FreeMode==game_mode)
        sprintf(info, "%s: 第 %d 关", mode_str(game_mode), cur_stage_idx);
    else
        sprintf(info, "%s", mode_str(game_mode));

    SendMessage(hwnd_statusbar, SB_SETTEXT,0, (LPARAM)info); 

    _stprintf (	info, TEXT("剩余格子: %d"), pt_board->nr_grids_left) ;
    SendMessage(hwnd_statusbar, SB_SETTEXT,1, (LPARAM)info); 

}


void update_statusbar_time()
{
    
    char info[128];
    char time_use[128];
    char time_left[128];

    if (!display_time)
    {
        info[0]=0;
        SendMessage(hwnd_statusbar, SB_SETTEXT,2, (LPARAM)info); 
        return;
    }

    time_str(time_use, game_use_time);
    time_str(time_left, game_left_time);

    _stprintf (	info
        , TEXT("用时: %s / 剩余: %s")
        , time_use
        , time_left) ;
    SendMessage(hwnd_statusbar, SB_SETTEXT,2, (LPARAM)info); 

}

void add_rollback(int row, int col, char value)
{
    int i;
    if (gt_rb_q.num>=MAX_GRID_NUM)
    {
        gt_board = gt_rb_q.at_steps[0];
        for (i = 0; i<MAX_GRID_NUM-1; i++)
            gt_rb_q.at_steps[i] = gt_rb_q.at_steps[i+1];

    }

    
    gt_rb_q.at_steps[gt_rb_q.num] = gt_board_tmp;

    gt_rb_q.ac_value[gt_rb_q.num] = value;
    gt_rb_q.at_pos[gt_rb_q.num] = make_pos(row, col);
    pt_board = &(gt_rb_q.at_steps[gt_rb_q.num]);
    gt_rb_q.num++;
    gt_rb_q.ac_value[gt_rb_q.num] = NO_VALUE_CHAR;

}



void get_grid_rect(RECT *rect, int i, int j)
{
    int xLeft, yTop, xRight, yBottom;
    xLeft = GAP_SIZE + (GAP_SIZE+grid_size)*j;
    yTop  = GAP_SIZE + (GAP_SIZE+grid_size)*i;

    if (i>=3) yTop+=GAP_SIZE;
    if (i>=6) yTop+=GAP_SIZE;

    if (j>=3) xLeft+=GAP_SIZE;
    if (j>=6) xLeft+=GAP_SIZE;

    xRight  = xLeft+grid_size;
    yBottom = yTop+grid_size;
    SetRect (rect, xLeft, yTop, xRight, yBottom) ;
}

int grid_on_focus(int row, int col)
{
    return (row==cur_row) && (col==cur_col);
}

int grid_pos_valid(int row, int col)
{
    return (row>=0) && (col>=0);
}

int no_grid_on_focus()
{
    return (-1==cur_row) && (-1==cur_col);
}

void * grid_mark_str(int row, int col)
{
    return at_grid[row][col].mark;
}


void refresh_grid(int row, int col)
{
    refresh_window(at_grid[row][col].hwnd);
}


void unfocus_any_grid()
{
    int old_row = cur_row;
    int old_col = cur_col;

    ShowWindow(hwnd_input_board, 0);

    cur_row=-1;
    cur_col=-1;

    if (grid_pos_valid(old_row, old_col))
        refresh_grid(old_row,old_col);
}

void refresh_nbs(int row, int col)
{
            t_pos_set nbs;
            int i;
            get_nbs(&nbs, row, col);
            for (i=0;i<nbs.num;i++)
                refresh_grid(nbs.at_pos[i].row, nbs.at_pos[i].col);
}

void focus_grid(int row, int col)
{
    int old_row = cur_row;
    int old_col = cur_col;

    cur_row=row;
    cur_col=col;
    
    //if (GetFocus()!=at_grid[cur_row][cur_col].hwnd)
        //SetFocus(at_grid[cur_row][cur_col].hwnd);

    ShowWindow(hwnd_input_board, 0);

    if (grid_pos_valid(old_row, old_col))
        refresh_grid(old_row,old_col);

    refresh_grid(cur_row,cur_col);
}

void blink_grid(int row, int col)
{
    at_grid[row][col].hBrush = hBrush_BLINK_1;
    refresh_grid(row, col);
    Sleep(250);
    at_grid[row][col].hBrush = hBrush_BLINK_2;
    refresh_grid(row, col);
    Sleep(250);
    at_grid[row][col].hBrush = hBrush_BLINK_1;
    refresh_grid(row, col);
    Sleep(250);
    at_grid[row][col].hBrush = hBrush_BLINK_2;
    refresh_grid(row, col);
    Sleep(250);
    at_grid[row][col].hBrush = NULL;
    //refresh_grid(row, col);
}

int can_un_do()
{
    return (gt_rb_q.num>0);
}

int can_re_do()
{
    if (gt_rb_q.num>=MAX_GRID_NUM) return 0;
    if (gt_rb_q.ac_value[gt_rb_q.num] == NO_VALUE_CHAR) return 0;
    return 1;
}

void un_do()
{
    t_pos *pt_pos;
    if (!can_un_do()) return;
    gt_rb_q.num--;
    pt_pos = &(gt_rb_q.at_pos[gt_rb_q.num]);
    if (gt_rb_q.num>0)
    {
        pt_board=&(gt_rb_q.at_steps[gt_rb_q.num-1]);
    }
    else
    {
        pt_board=&gt_board;
    }
    
    focus_grid(pt_pos->row, pt_pos->col);
    refresh_nbs(pt_pos->row, pt_pos->col);

    set_toolbar_button(IDT_TOOLBAR_REDO, 1);
    update_statusbar();
}

void re_do()
{
    t_pos *pt_pos;
    if (!can_re_do()) return;
    pt_board=&(gt_rb_q.at_steps[gt_rb_q.num]);
    pt_pos = &(gt_rb_q.at_pos[gt_rb_q.num]);
    focus_grid(pt_pos->row, pt_pos->col);
    refresh_nbs(pt_pos->row, pt_pos->col);
    gt_rb_q.num++;
    set_toolbar_button(IDT_TOOLBAR_UNDO, 1);
    update_statusbar();
}



void step_to_next_stage()
{
    cur_stage_idx++;
    load_stage(cur_stage, cur_stage_idx);
    InitNewGame(cur_stage);
    refresh_board();

    update_statusbar();

    KillTimer(hwnd_main_board, TIMER_GAME_USE_TIME_CNT) ;
    game_use_time = 0;
    game_left_time = 7*DAY_SEC;
    update_statusbar_time();

    SetTimer(hwnd_main_board, TIMER_GAME_USE_TIME_CNT, 1000, NULL);

}

void proc_digit_input(char value)
{
    int    char_int = value-'0';


    {

        if (!grid_has_candi(pt_board, cur_row, cur_col, char_int+'0'))
        {
            //WinPrintf(hwnd_frame,TEXT("错误"), TEXT("输入数字与已有数字冲突"));
            nbs_have_value(pt_board, cur_row, cur_col, char_int+'0');
            play_sound_async(TEXT("sd_wrong"),  SND_RESOURCE);
            blink_grid(pt_board->tmp_pos.row, pt_board->tmp_pos.col);
            return;
        }

        if (nbs_have_only_this_candi(pt_board, cur_row, cur_col, char_int+'0'))
        {
            //WinPrintf(hwnd_frame,TEXT("错误"), TEXT("输入数字与已有数字冲突"));
            //at_grid[row][ col]->BlinkPaint(wxT("0"));
            play_sound_async(TEXT("sd_wrong"),  SND_RESOURCE);
            blink_grid(pt_board->tmp_pos.row, pt_board->tmp_pos.col);
            return;
        }

    gt_board_tmp = *pt_board;
    assign_grid(&gt_board_tmp, cur_row, cur_col, value);

        add_rollback(cur_row, cur_col, char_int+'0');
        play_sound_async(TEXT("sd_right"),  SND_RESOURCE);
        refresh_grid(cur_row,cur_col);
        set_toolbar_button(IDT_TOOLBAR_UNDO, 1);
        update_statusbar();

        if (2==display_help)
        {
            refresh_nbs(cur_row, cur_col);
        }


        //if (UsrDefMode==game_mode) return;
#if 1
        if (board_solved(pt_board))
        {
            KillTimer(hwnd_main_board, TIMER_GAME_USE_TIME_CNT) ;

            if (StageMode!=game_mode)
            {
                game_over=1;
                play_sound_async(TEXT("sd_stage_succ"),  SND_RESOURCE);
                WinPrintf(hwnd_frame,TEXT("恭喜"), TEXT("恭喜恭喜！%s 胜利！"), mode_str(game_mode));
                return 1;
            }


            if (cur_stage_idx==nr_stages)
            {
                game_over=1;
                play_sound_async(TEXT("sd_all_stages_succ"),  SND_RESOURCE);
                WinPrintf(hwnd_frame,TEXT("恭喜"), TEXT("你太牛了！%s 通关！"), mode_str(game_mode));
                return 1;
            }

            if (!game_over)
            {
                play_sound_async(TEXT("sd_stage_succ"),  SND_RESOURCE);
                WinPrintf(hwnd_frame,TEXT("恭喜"), TEXT("恭喜恭喜！闯入下一关！"));
                step_to_next_stage();
                InvalidateRect(hwnd_main_board, NULL, TRUE);
            }
        }
#endif



    }

}

void proc_modify_input(char value)
{
    int    char_int = value-'0';
    char  buf[128];

    board_to_input_str(buf, pt_board);
    buf[cur_row*9 + cur_col] = '.';
    load_game(&gt_board_tmp, buf);


    {



        if (!grid_has_candi(&gt_board_tmp, cur_row, cur_col, char_int+'0'))
        {
            //WinPrintf(hwnd_frame,TEXT("错误"), TEXT("输入数字与已有数字冲突"));
            nbs_have_value(&gt_board_tmp, cur_row, cur_col, char_int+'0');
            play_sound_async(TEXT("sd_wrong"),  SND_RESOURCE);
            blink_grid(gt_board_tmp.tmp_pos.row, gt_board_tmp.tmp_pos.col);
            return;
        }

        if (nbs_have_only_this_candi(&gt_board_tmp, cur_row, cur_col, char_int+'0'))
        {
            //WinPrintf(hwnd_frame,TEXT("错误"), TEXT("输入数字与已有数字冲突"));
            //at_grid[row][ col]->BlinkPaint(wxT("0"));
            play_sound_async(TEXT("sd_wrong"),  SND_RESOURCE);
            blink_grid(gt_board_tmp.tmp_pos.row, gt_board_tmp.tmp_pos.col);
            return;
        }

        assign_grid(&gt_board_tmp, cur_row, cur_col, value);
        add_rollback(cur_row, cur_col, char_int+'0');

        play_sound_async(TEXT("sd_right"),  SND_RESOURCE);
        refresh_nbs(cur_row, cur_col);
        set_toolbar_button(IDT_TOOLBAR_UNDO, 1);
        update_statusbar();

        if (2==display_help)
        {
            refresh_nbs(cur_row, cur_col);
        }


    }

}

TCHAR szGridWinClassName[] = TEXT ("grid_win") ;
LRESULT CALLBACK grid_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc ;
    PAINTSTRUCT ps ;
    RECT		rect ;
    int cxClient, cyClient;
    int old_row = cur_row;
    int old_col = cur_col;

    t_grid_info *pt_grid_info = NULL;
    int row = 0;
    int col = 0;

    POINT point ;

    if (message==WM_PAINT || 
        message==WM_LBUTTONDOWN ||
        message==WM_RBUTTONDOWN ||
        message==WM_LBUTTONDBLCLK)
    {
        pt_grid_info = get_grid_info(hwnd);
        row = pt_grid_info->row;
        col = pt_grid_info->col;
    }

    
    switch (message)
    {
        case WM_CREATE:

            return 0 ;

        case WM_PAINT :
        {
            char str[32];
            HBRUSH hBrush;

            SetRect(&rect, 0, 0, grid_size, grid_size);

            if (pt_grid_info->hBrush != NULL)
                hBrush = pt_grid_info->hBrush;
            else if (grid_on_focus(row, col))
                hBrush = hBrush_ON_FOCUS;
            else if (grid_has_value(pt_board, row, col))
            {
                if (pt_grid_info->input_by_user)
                    hBrush = hBrush_INPUT_BY_USER;
                else
                    hBrush = hBrush_HAS_VALUE;
            }
            else
                hBrush = hBrush_NO_VALUE;
            
            hdc = BeginPaint (hwnd, &ps) ;

            FillRect (hdc, &rect, hBrush) ;


            if (grid_has_value(pt_board, row, col))
            {
                sprintf(str, "%c", pt_board->at_grid[row][col].value);

                fw_text_out_middle_trans(hdc
                    , &rect, big_font_size, RGB(0x0,0x0,0x0)
                    , str, 1);
            }
            else if (2==display_help)
            {
                sprintf(str, "%s ", pt_board->at_grid[row][col].ac_candi);
                SetRect(&rect, 0, 0, grid_size, grid_size/2);
                fw_text_out_middle_trans(hdc
                    , &rect, small_font_size, RGB(0x0,0x0,0x0)
                    , str, 5);

                SetRect(&rect, 0, grid_size/2, grid_size, grid_size);
                fw_text_out_middle_trans(hdc
                    , &rect, small_font_size, RGB(0x0,0x0,0x0)
                    , str+5, 5);

            }
            else if (1==display_help && 
                pt_grid_info->input_by_user &&
                strcmp(pt_grid_info->mark, "123456789 ") )
            {
                SetRect(&rect, 0, 0, grid_size, grid_size/2);
                fw_text_out_middle_trans(hdc
                    , &rect, small_font_size, RGB(0xff,0xff,0xff)
                    , pt_grid_info->mark, 5);

                SetRect(&rect, 0, grid_size/2, grid_size, grid_size);
                fw_text_out_middle_trans(hdc
                    , &rect, small_font_size, RGB(0xff,0xff,0xff)
                    , pt_grid_info->mark+5, 5);

            }

            
            EndPaint (hwnd, &ps) ;

            if (grid_on_focus(row, col) && 
                IsWindowVisible(hwnd_input_board)) 
            {
                refresh_window(hwnd_input_board);
            }
            return 0 ;
        }


        case WM_LBUTTONDOWN:
        {
            if (!grid_on_focus(row, col))
            {
                focus_grid(row, col);
            }
            else if ((pt_grid_info->input_by_user) && 
                !IsWindowVisible(hwnd_input_board))
            {
                display_input_win(row, col);
            }

            return 0;
        }

        case WM_LBUTTONDBLCLK:
        {
            if ((pt_grid_info->input_by_user) && 
                !IsWindowVisible(hwnd_input_board))
            {
                display_input_win(row, col);
            }

            return 0;
        }

        case WM_RBUTTONDOWN:
        {
            if (!grid_on_focus(row, col))
            {
                focus_grid(row, col);
            }
            
            if ((pt_grid_info->input_by_user) && 
                !IsWindowVisible(hwnd_input_board))
            {
                display_mark_win(row, col);
            }

            return 0;
        }



        case WM_CHAR:
        {

            SendMessage(hwnd_main_board, WM_CHAR, wParam, NULL);
            return 0 ;

        }

        case WM_KEYDOWN:
        {

            SendMessage(hwnd_main_board, WM_KEYDOWN, wParam, NULL);
            return 0 ;

        }


    }



    
    return DefWindowProc(hwnd, message, wParam, lParam) ;
}

int register_grid_win()
{
    WNDCLASS    sub_wndclass;
    sub_wndclass.style      = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    sub_wndclass.lpfnWndProc= grid_WndProc;
    sub_wndclass.cbClsExtra = 0;
    sub_wndclass.cbWndExtra = 0;
    sub_wndclass.hInstance  = g_hInstance;
    sub_wndclass.hIcon      = NULL;
    sub_wndclass.hCursor    = LoadCursor (NULL, IDC_ARROW);
    sub_wndclass.hbrBackground  = (HBRUSH)GetStockObject (WHITE_BRUSH);
    sub_wndclass.lpszMenuName   = NULL;
    sub_wndclass.lpszClassName  = szGridWinClassName;


    if (!RegisterClass (&sub_wndclass))
     {
        MessageBox (NULL, TEXT ("Program requires Windows NT!"),
          szAppName, MB_ICONERROR) ;
        return FAIL;
     }

    return SUCCESS;

}

int stage_mode_begin()
{
    game_mode=StageMode;
    game_over=0;
    cur_stage_idx=0;
    step_to_next_stage();

}


void toolbar_redo_undo_init()
{
    if (!can_un_do())
        set_toolbar_button(IDT_TOOLBAR_UNDO, 0);
    else
        set_toolbar_button(IDT_TOOLBAR_UNDO, 1);

    if (!can_re_do())
        set_toolbar_button(IDT_TOOLBAR_REDO, 0);
    else
        set_toolbar_button(IDT_TOOLBAR_REDO, 1);

}

void load_puzzle(char *output, char *file_path)
{
    char buf[128]={0};
    int cnt = 0;
    char *pchar;

    FILE *fp=fopen(file_path,"r");
    if (NULL==fp) return;

    while (cnt<81)
    {
        fgets(buf, sizeof(buf), fp);
        pchar = buf;
        while ((*pchar)!=0)
        {
            if ((*pchar) == '.' || ((*pchar) >= '1' && (*pchar) <= '9'))
            {
                output[cnt] = *pchar;
                cnt++;
            }
            pchar++;
        }
    }

    output[cnt] = 0;
    fclose(fp);

}

void load_puzzle_as_game(char *file_path)
{
    game_mode=UsrLoadMode;
    game_over=0;
    load_puzzle(cur_stage, file_path);
    InitNewGame(cur_stage);
    refresh_board();

    update_statusbar();

    KillTimer(hwnd_main_board, TIMER_GAME_USE_TIME_CNT) ;
    game_use_time = 0;
    game_left_time = 7*DAY_SEC;
    update_statusbar_time();

    SetTimer(hwnd_main_board, TIMER_GAME_USE_TIME_CNT, 1000, NULL);
}

void game_to_hm_str(char * output)
{
    int i;
    char buf[128];
    char buf_2[16] = {0};
    output[0] = 0;

    buf_2[9] = '\r';
    buf_2[10] = '\n';

    board_to_input_str(buf, pt_board);

    for (i=0; i<MAX_ROW_NUM; i++)
    {
        memcpy(buf_2, buf+i*9, 9);
        strcat(output, buf_2);
    }

}

void SaveAsPuzzle(char *file_path)
{
    int i;
    char buf[128];
    char buf_2[16]={0};

    FILE *fp = fopen(file_path, "w");

    board_to_input_str(buf, pt_board);

    for (i=0; i<MAX_ROW_NUM; i++)
    {
        memcpy(buf_2, buf+i*9, 9);
        fputs(buf_2, fp);
        fputs("\n", fp);
    }
   
    fclose(fp);

}

void SaveAsArch(char *file_path)
{
    int i, j;
    int row, col;
    char buf[128]={0};

    FILE *fp = fopen(file_path, "w");
    sprintf(buf, "v%c.%c.%c.%c \r\n"
        , version[0], version[1],version[2],version[3]);
    fputs(buf, fp);

    sprintf(buf, "mode=%d \r\n", game_mode);
    fputs(buf, fp);
    sprintf(buf, "stage_idx=%d \r\n", cur_stage_idx);
    fputs(buf, fp);

    sprintf(buf, "time_used=%d \r\n", game_use_time);
    fputs(buf, fp);
    sprintf(buf, "time_left=%d \r\n", game_left_time);
    fputs(buf, fp);

    for (i=0; i<MAX_GRID_NUM; i++)
    {
        row = i/MAX_COL_NUM;
        col = i%MAX_COL_NUM;

        if (grid_has_value(pt_board, row, col)
            &&at_grid[row][col].input_by_user==0)
            buf[i] = grid_value(pt_board, row, col);
        else
            buf[i] = NO_VALUE_CHAR;

    }

    int ret = fputs(buf, fp);
    fputs("\r\n", fp);
    sprintf(buf, "finish_grid_num=%d \r\n", gt_rb_q.num);
    fputs(buf, fp);

    for (row=0; row<MAX_ROW_NUM; row++)
        for (col=0; col<MAX_COL_NUM; col++)
    {

        if (grid_has_value(pt_board, row, col)
            &&at_grid[row][col].input_by_user)
            {
        sprintf(buf, "%d %d %c\r\n"
            , row
            , col
            ,grid_value(pt_board, row, col));
        fputs(buf, fp);

            }

    }


    fclose(fp);
    update_file_open_history(file_path);
}

void LoadArch(char *file_path)
{
    int i, j;
    int row, col;
    int nr_input;
    char value;

    char buf[128]={0};

    FILE *fp=fopen(file_path,"r");
    if (NULL==fp) return;

    KillTimer(hwnd_main_board, TIMER_GAME_USE_TIME_CNT) ;

    fgets(buf, sizeof(buf), fp);
    if (buf[0]!='v' || buf[1]!=version[0])
    {
        WinPrintf(hwnd_frame, TEXT("错误"), TEXT("存档文件版本不匹配"));
        fclose(fp);
        return;

    }

    fgets(buf, sizeof(buf), fp);
    sscanf(buf, "mode=%d \r\n", &game_mode);
    fgets(buf, sizeof(buf), fp);
    sscanf(buf, "stage_idx=%d \r\n", &cur_stage_idx);
    fgets(buf, sizeof(buf), fp);
    sscanf(buf, "time_used=%d \r\n", &game_use_time);
    fgets(buf, sizeof(buf), fp);
    sscanf(buf, "time_left=%d \r\n", &game_left_time);

    fgets(buf, sizeof(buf), fp);
    InitNewGame(buf);

    fgets(buf, sizeof(buf), fp);
    sscanf(buf, "finish_grid_num=%d \r\n", &nr_input);

    for (i=0;i<nr_input;i++)
    {
        fgets(buf, sizeof(buf), fp);
        sscanf(buf, "%d %d %c\r\n", &row, &col, &value);
        assign_grid(pt_board, row, col, value);
    }

    fclose(fp);

    refresh_board();
    toolbar_redo_undo_init();
    update_statusbar();
    update_statusbar_time();

    SetTimer(hwnd_main_board, TIMER_GAME_USE_TIME_CNT, 1000, NULL);
    
    update_file_open_history(file_path);

}

void select_empty_game()
{
    game_mode=EmptyMode;
    game_over=0;
    cur_stage_idx=0;
    InitNewGame(empty_stage);
    refresh_board();

    update_statusbar();

    KillTimer(hwnd_main_board, TIMER_GAME_USE_TIME_CNT) ;
    game_use_time = 0;
    game_left_time = 7*DAY_SEC;
    update_statusbar_time();

    SetTimer(hwnd_main_board, TIMER_GAME_USE_TIME_CNT, 1000, NULL);

}

void LoadRandGame()
{
    if (generate_puzzle(tmp_stage)==IDCANCEL)
        return;

    game_mode=RandomMode;
    game_over=0;
    cur_stage_idx=0;


    InitNewGame(tmp_stage);
    refresh_board();

    update_statusbar();

    KillTimer(hwnd_main_board, TIMER_GAME_USE_TIME_CNT) ;
    game_use_time = 0;
    game_left_time = 7*DAY_SEC;
    update_statusbar_time();

    SetTimer(hwnd_main_board, TIMER_GAME_USE_TIME_CNT, 1000, NULL);

}
void select_stage(int stage_idx)
{
    game_mode=FreeMode;
    game_over=0;
    cur_stage_idx=stage_idx;
    load_stage(cur_stage, cur_stage_idx);
    InitNewGame(cur_stage);
    refresh_board();

    update_statusbar();

    KillTimer(hwnd_main_board, TIMER_GAME_USE_TIME_CNT) ;
    game_use_time = 0;
    game_left_time = 7*DAY_SEC;
    update_statusbar_time();

    SetTimer(hwnd_main_board, TIMER_GAME_USE_TIME_CNT, 1000, NULL);
}

void gen_stage_menus(int stage_num)
{
    int i;
    char field_value[MAX_FILE_PATH_LEN];
    char    menu_name[MAX_FILE_PATH_LEN + 8];
    
    HMENU hMenu = GetMenu(hwnd_frame);
                hMenu = GetSubMenu(hMenu, 0);
                hMenu = GetSubMenu(hMenu, 1);

        DeleteMenu(hMenu, 0, MF_BYPOSITION);



    for (i=1; i<=stage_num; i++)
    {
        sprintf(menu_name, "&%d  第 %d 关", i, i);
        AppendMenu(hMenu, MF_STRING, ID_STAGE_1+i-1,  menu_name) ;
    }

}

LRESULT CALLBACK main_board_WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc ;
    PAINTSTRUCT ps ;
    RECT		rect ;
    int cxClient, cyClient;
    
    POINT point ;
    
    switch (message)
    {
        case WM_CREATE:
        {
            int i,j;
            InitStageNum();
            gen_stage_menus(nr_stages);

            hwnd_main_board = hwnd;

            hwnd_input_board = CreateWindow (szInputBoardWinClassName, TEXT ("input_board"),
                    WS_CHILD,
                    CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
                    hwnd, NULL, g_hInstance, NULL) ;

            InitGrids();

            hBrush_NO_VALUE = CreateSolidBrush (COLOUR_NO_VALUE) ;
            hBrush_HAS_VALUE = CreateSolidBrush (COLOUR_HAS_VALUE) ;
            hBrush_INPUT_BY_USER = CreateSolidBrush (COLOUR_INPUT_BY_USER) ;
            hBrush_ON_FOCUS = CreateSolidBrush (COLOUR_ON_FOCUS) ;
            hBrush_BLINK_1 = CreateSolidBrush (COLOUR_BLINK_1) ;
            hBrush_BLINK_2 = CreateSolidBrush (COLOUR_BLINK_2) ;


            for (i=0;i<9;i++)
                for (j=0;j<9;j++)
            {
                at_grid[i][j].hwnd = CreateWindow (szGridWinClassName, TEXT ("grid"),
                    WS_CHILD,
                    CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
                    hwnd, NULL, g_hInstance, NULL) ;

                ShowWindow (at_grid[i][j].hwnd, 1) ;
                UpdateWindow (at_grid[i][j].hwnd) ;

            }
                
            stage_mode_begin();

            mciSendString("open music.mp3 type mpegvideo alias music", NULL, 0, NULL);
            //mciSendString("open music.MID type sequencer alias music", NULL, 0, NULL);
            mciSendString("play music repeat", NULL, 0, NULL);
            return 0 ;
        }


        case WM_SIZE:
        {
            int i, j;
      		cxClient = LOWORD (lParam) ;
      		cyClient = HIWORD (lParam) ;
            win_size = cxClient;
            grid_size = (win_size - GAP_SIZE*12)/9;
            big_font_size = grid_size;
            small_font_size = grid_size/3;
            
            for (i=0;i<9;i++)
                for (j=0;j<9;j++)
            {
                get_grid_rect(&rect, i, j);

                MoveWindow(at_grid[i][j].hwnd, 	
                rect.left,  
                rect.top,
                grid_size, 
                grid_size, 
                TRUE) ;

            }

            if (IsWindowVisible(hwnd_input_board))
                mov_input_win_to_grid(cur_row, cur_col);

            return 0 ;
        }

        case WM_LBUTTONDOWN:
        {
            unfocus_any_grid();
            return 0;
        }

        case WM_CHAR:
        {
            ShowWindow(hwnd_input_board, 0);
            if (no_grid_on_focus())
            {
                show_tip(TEXT("请先选中一个方格"));
                return 0;
            }
            if (wParam<'1' || wParam>'9')
            {
                WinPrintf(hwnd, TEXT("提醒"), TEXT("请输入数字1~9"));
                return 0;
            }
            
            if (board_solved(pt_board))
            {
                WinPrintf(hwnd_frame,TEXT("提醒"), TEXT("游戏已经结束"));
                return 0;
            }

            if (!grid_has_value(pt_board, cur_row, cur_col))
                proc_digit_input(wParam);
            else if (at_grid[cur_row][cur_col].input_by_user &&
                wParam != grid_value(pt_board, cur_row, cur_col))
                proc_modify_input(wParam);

            return 0 ;

        }

        case WM_KEYDOWN:
        {
            int old_row = cur_row, old_col = cur_col;
            ShowWindow(hwnd_input_board, 0);

            switch (wParam)
            {
                case VK_LEFT:
                case VK_RIGHT:
                case VK_UP:
                case VK_DOWN:

                if (no_grid_on_focus()) 
                {
                    WinPrintf(hwnd, TEXT("提醒"), TEXT("请先选中一个方格"));
                    return 0;
                }
                break;
                
                default:
                    return 0;

            }

            switch (wParam)
            {
                case VK_LEFT:
                {
                    if (cur_col>0) cur_col-=1;
                    break;
                }
                case VK_RIGHT:
                {
                    if (cur_col<8) cur_col+=1;
                    break;
                }
                case VK_UP:
                {
                    if (cur_row>0) cur_row-=1;
                    break;
                }
                case VK_DOWN:
                {
                    if (cur_row<8) cur_row+=1;
                    break;
                }

            }

            
            InvalidateRect(at_grid[old_row][old_col].hwnd, NULL, TRUE);
            InvalidateRect(at_grid[cur_row][cur_col].hwnd, NULL, TRUE);
            return 0;

        }

        case	WM_TIMER:
        {
            game_use_time++;
            game_left_time--;
            update_statusbar_time();
            return 0;
        }

    }
    
    return DefWindowProc (hwnd, message, wParam, lParam) ;
}

int register_main_board_win()
{
    WNDCLASS    sub_wndclass;
    sub_wndclass.style      = CS_HREDRAW | CS_VREDRAW;
    sub_wndclass.lpfnWndProc= main_board_WndProc;
    sub_wndclass.cbClsExtra = 0;
    sub_wndclass.cbWndExtra = 0;
    sub_wndclass.hInstance  = g_hInstance;
    sub_wndclass.hIcon      = NULL;
    sub_wndclass.hCursor    = LoadCursor (NULL, IDC_ARROW);
    sub_wndclass.hbrBackground  = (HBRUSH)CreateSolidBrush(GetSysColor(COLOR_ACTIVECAPTION));
    sub_wndclass.lpszMenuName   = NULL;
    sub_wndclass.lpszClassName  = szMainBoardWinClassName;


    if (!RegisterClass (&sub_wndclass))
     {
        MessageBox (NULL, TEXT ("Program requires Windows NT!"),
          szAppName, MB_ICONERROR) ;
        return FAIL;
     }

    return SUCCESS;

}


