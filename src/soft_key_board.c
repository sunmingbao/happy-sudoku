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

#include "common.h"
#include "utils.h"
#include "ext_utils_textout.h"


#define    COLOUR_DELETED            RGB(0x00,0x00,0xff)
#define    COLOUR_NOT_DELETED        RGB(0xff,0xff,0xff)

#define    COLOUR_SELECTED            RGB(0xff,0x00,0x00)
#define    COLOUR_NOT_SELECTED        RGB(0xff,0xff,0xff)

HBRUSH hBrush_NOT_DELETED, hBrush_DELETED;
HBRUSH hBrush_NOT_SELECTED, hBrush_SELECTED;
static int grid_width, grid_height, font_size;

int win_type;
char *mark_str;

typedef struct
{
    HWND hwnd;
    int  row;
    int  col;
    char val[2];
}t_input_grid_info;

t_input_grid_info at_input_grid[2][5];

static int cur_input_row=-1, cur_input_col=-1;

void *get_input_grid_info(HWND hwnd)
{
    int i, j;
    for (i=0;i<2;i++)
        for (j=0;j<5;j++)
    {
        if (at_input_grid[i][j].hwnd == hwnd)
            return &(at_input_grid[i][j]);

    }


    return NULL;
}


void InitInputGrids()
{

    int row, col;

    for (row=0; row<2; row++)
        for (col=0; col<5; col++)
    {

        at_input_grid[row][col].row = row;
        at_input_grid[row][col].col = col;
        at_input_grid[row][col].val[0]= row*5+col+'1';
    }

        at_input_grid[1][4].val[0]= 'Y';

}


void get_input_grid_rect(RECT *rect, int i, int j)
{
    int xLeft, yTop, xRight, yBottom;
    xLeft = grid_width*j;
    yTop  = grid_height*i;

    xRight  = xLeft+grid_width;
    yBottom = yTop+grid_height;
    SetRect (rect, xLeft, yTop, xRight, yBottom) ;
}

int input_grid_on_focus(int row, int col)
{
    return (row==cur_input_row) && (col==cur_input_col);
}

int input_grid_pos_valid(int row, int col)
{
    return (row>=0) && (col>=0);
}

int no_input_grid_on_focus()
{
    return (-1==cur_input_row) && (-1==cur_input_col);
}

char focus_grid_char()
{
    return at_input_grid[cur_input_row][cur_input_col].val[0];
}

int is_last_input_grid(int row, int col)
{
    return (row==1) && (col==4);
}

int input_grid_idx(int row, int col)
{
    return row*5 + col;
}

void refresh_input_grid(int row, int col)
{
    refresh_window(at_input_grid[row][col].hwnd);
}

void unfocus_any_input_grid()
{
    int old_row = cur_input_row;
    int old_col = cur_input_col;

    cur_input_row=-1;
    cur_input_col=-1;

    if (input_grid_pos_valid(old_row, old_col))
        refresh_input_grid(old_row,old_col);
}

void focus_input_grid(int row, int col)
{
    int old_row = cur_input_row;
    int old_col = cur_input_col;

    cur_input_row=row;
    cur_input_col=col;

    if (input_grid_pos_valid(old_row, old_col))
        refresh_input_grid(old_row,old_col);

    refresh_input_grid(cur_input_row,cur_input_col);
}


TCHAR szInputGridWinClassName[] = TEXT ("input_grid_win") ;

void mark_win_paint(HDC hdc, RECT *rect, t_input_grid_info *pt_grid_info)
{
       int row = pt_grid_info->row;
       int col = pt_grid_info->col;
       int idx = input_grid_idx(row, col);
       char value = idx + 1 + '0';
       HBRUSH hBrush;
       if (mark_str[idx] >= '1' && mark_str[idx] <= '9')
            hBrush = hBrush_NOT_DELETED;
       else if (is_last_input_grid(row, col))
            hBrush = hBrush_NOT_DELETED;
       else
            hBrush = hBrush_DELETED;


        FillRect(hdc, rect, hBrush) ;

        fw_text_out_middle_trans(hdc
                , rect, font_size, RGB(0x0,0x0,0x0)
                , pt_grid_info->val, 1);


}

void input_win_paint(HDC hdc, RECT *rect, t_input_grid_info *pt_grid_info)
{
       int row = pt_grid_info->row;
       int col = pt_grid_info->col;
       HBRUSH hBrush;

       if (input_grid_on_focus(row, col))
            hBrush = hBrush_SELECTED;
       else
            hBrush = hBrush_NOT_SELECTED;


            FillRect(hdc, rect, hBrush) ;

            fw_text_out_middle_trans(hdc
                    , rect, font_size, RGB(0x0,0x0,0x0)
                    , pt_grid_info->val, 1);

}

void hide_input_window()
{
    ShowWindow(hwnd_input_board, 0);
    refresh_grid_on_focus();
}

LRESULT CALLBACK input_grid_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc ;
    PAINTSTRUCT ps ;
    RECT		rect ;
    int cxClient, cyClient;

    t_input_grid_info *pt_grid_info = NULL;
    int row = 0;
    int col = 0;
    int old_row = cur_input_row;
    int old_col = cur_input_col;

    POINT point ;

    if (message==WM_PAINT || 
        message==WM_LBUTTONDOWN ||
        message==WM_LBUTTONDBLCLK)
    {
        pt_grid_info = get_input_grid_info(hwnd);
        row = pt_grid_info->row;
        col = pt_grid_info->col;
    }

    
    switch (message)
    {
        case WM_CREATE:

            return 0 ;

        case WM_PAINT :
        {

            SetRect(&rect, 0, 0, grid_width, grid_height);

            
            hdc = BeginPaint (hwnd, &ps) ;



            if (win_type==0)
            {
                input_win_paint(hdc, &rect, pt_grid_info);
            }

            else
                mark_win_paint(hdc, &rect, pt_grid_info);
            
            EndPaint (hwnd, &ps) ;
            return 0 ;
        }


        case WM_LBUTTONDOWN:
        {
            
            if (is_last_input_grid(row, col))
            {
                if (!no_input_grid_on_focus())
                {
                    hide_input_window();
                    SendMessage(hwnd_main_board, WM_CHAR, focus_grid_char(), NULL);
                }

                return 0;

            }

            if (input_grid_on_focus(row, col))
            {
                hide_input_window();
                SendMessage(hwnd_main_board, WM_CHAR, focus_grid_char(), NULL);
                return 0;
            }

            cur_input_row = pt_grid_info->row;
            cur_input_col = pt_grid_info->col;

            if (input_grid_pos_valid(old_row, old_col))
            {
                refresh_input_grid(old_row, old_col);
            }
            
            refresh_window(hwnd);

            return 0;
        }

        case WM_LBUTTONDBLCLK:
        {
            if (win_type==1) return 0;
            
            if (is_last_input_grid(row, col))
            {
                if (!no_input_grid_on_focus())
                {
                    hide_input_window();
                    SendMessage(hwnd_main_board, WM_CHAR, focus_grid_char(), NULL);
                }

                return 0;

            }

            hide_input_window();
            SendMessage(hwnd_main_board, WM_CHAR, focus_grid_char(), NULL);

            return 0;
        }

        

    }



    
    return DefWindowProc(hwnd, message, wParam, lParam) ;
}

int register_input_grid_win()
{
    WNDCLASS    sub_wndclass;
    sub_wndclass.style      = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    sub_wndclass.lpfnWndProc= input_grid_WndProc;
    sub_wndclass.cbClsExtra = 0;
    sub_wndclass.cbWndExtra = 0;
    sub_wndclass.hInstance  = g_hInstance;
    sub_wndclass.hIcon      = NULL;
    sub_wndclass.hCursor    = LoadCursor (NULL, IDC_ARROW);
    sub_wndclass.hbrBackground  =  (HBRUSH)GetStockObject (WHITE_BRUSH);
    sub_wndclass.lpszMenuName   = NULL;
    sub_wndclass.lpszClassName  = szInputGridWinClassName;


    if (!RegisterClass (&sub_wndclass))
     {
        MessageBox (NULL, TEXT ("Program requires Windows NT!"),
          szAppName, MB_ICONERROR) ;
        return FAIL;
     }

    return SUCCESS;

}

void hide_input_mark_win()
{
    ShowWindow(hwnd_input_board, 0);
    ShowWindow(hwnd_mark_board, 0);
}

void refresh_input_mark_win()
{
    if (IsWindowVisible(hwnd_input_board))
        refresh_window(hwnd_input_board);

    if (IsWindowVisible(hwnd_mark_board))
        refresh_window(hwnd_mark_board);
}

void mov_input_mark_win_to_grid(int row, int col)
{
    RECT		rect ;

    
    get_grid_rect(&rect, row, col);

    rect.left -= GAP_SIZE;
    rect.top  -= GAP_SIZE;
    rect.right  += GAP_SIZE;
    rect.bottom += GAP_SIZE;
#if 1
    MoveWindow(hwnd_input_board, 	
        rect.left,  
        rect.top, 
        rect.right - rect.left, 
        rect.bottom - rect.top, 
        TRUE);
    MoveWindow(hwnd_mark_board, 	
        rect.left,  
        rect.top, 
        rect.right - rect.left, 
        rect.bottom - rect.top, 
        TRUE);
#else
SetWindowPos(
hwnd_input_board,
HWND_TOP,
        rect.left,  
        rect.top, 
        rect.right - rect.left, 
        rect.bottom - rect.top, 
SWP_HIDEWINDOW
);
#endif
}

void display_input_win(int row, int col)
{
    win_type = 0;
    mov_input_mark_win_to_grid(row, col);
    unfocus_any_input_grid();
    ShowWindow(hwnd_input_board, 1);
}

void display_mark_win(int row, int col)
{
    win_type = 1;
    mark_str=grid_mark_str(row, col);
    mov_input_mark_win_to_grid(row, col);
    ShowWindow(hwnd_mark_board, 1);
}

TCHAR szInputBoardWinClassName[] = TEXT ("input_board") ;
TCHAR szMarkBoardWinClassName[] = TEXT ("mark_board") ;
HWND    hwnd_input_board;
HWND    hwnd_mark_board;
LRESULT CALLBACK input_board_WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
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

            hwnd_input_board = hwnd;

            InitInputGrids();

            hBrush_NOT_DELETED= CreateSolidBrush (COLOUR_NOT_DELETED) ;
            hBrush_DELETED= CreateSolidBrush (COLOUR_DELETED) ;
            hBrush_NOT_SELECTED= CreateSolidBrush (COLOUR_NOT_SELECTED) ;
            hBrush_SELECTED= CreateSolidBrush (COLOUR_SELECTED) ;


            for (i=0;i<2;i++)
                for (j=0;j<5;j++)
            {
                at_input_grid[i][j].hwnd = CreateWindow (szInputGridWinClassName, TEXT ("grid"),
                    WS_CHILD,
                    CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
                    hwnd, NULL, g_hInstance, NULL) ;

                ShowWindow (at_input_grid[i][j].hwnd, 1) ;
                UpdateWindow (at_input_grid[i][j].hwnd) ;

            }

            return 0 ;
        }


        case WM_SIZE:
        {
            int i, j;
      		cxClient = LOWORD (lParam) ;
      		cyClient = HIWORD (lParam) ;
            grid_width = cxClient/5;
            grid_height= cyClient/2;
            font_size = grid_height-5;
            
            for (i=0;i<2;i++)
                for (j=0;j<5;j++)
            {
                get_input_grid_rect(&rect, i, j);

                MoveWindow(at_input_grid[i][j].hwnd, 	
                rect.left,  
                rect.top,
                grid_width, 
                grid_height, 
                TRUE) ;

            }


            return 0 ;
        }

        
    }
    
    return DefWindowProc (hwnd, message, wParam, lParam) ;
}

int register_input_board_win()
{
    WNDCLASS    sub_wndclass;
    sub_wndclass.style      = CS_HREDRAW | CS_VREDRAW;
    sub_wndclass.lpfnWndProc= input_board_WndProc;
    sub_wndclass.cbClsExtra = 0;
    sub_wndclass.cbWndExtra = 0;
    sub_wndclass.hInstance  = g_hInstance;
    sub_wndclass.hIcon      = NULL;
    sub_wndclass.hCursor    = LoadCursor (NULL, IDC_ARROW);
    sub_wndclass.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
    sub_wndclass.lpszMenuName   = NULL;
    sub_wndclass.lpszClassName  = szInputBoardWinClassName;


    if (!RegisterClass (&sub_wndclass))
     {
        MessageBox (NULL, TEXT ("Program requires Windows NT!"),
          szAppName, MB_ICONERROR) ;
        return FAIL;
     }

    return SUCCESS;

}

typedef struct
{
    HWND hwnd;
    int  row;
    int  col;
    char val[2];
}t_mark_grid_info;

t_mark_grid_info at_mark_grid[2][5];

void InitMarkGrids()
{

    int row, col;

    for (row=0; row<2; row++)
        for (col=0; col<5; col++)
    {

        at_mark_grid[row][col].row = row;
        at_mark_grid[row][col].col = col;
        at_mark_grid[row][col].val[0]= row*5+col+'1';
    }

        at_mark_grid[1][4].val[0]= 'Y';

}

TCHAR szMarkGridWinClassName[] = TEXT ("mark_grid_win") ;



void hide_mark_window()
{
    ShowWindow(hwnd_mark_board, 0);
    refresh_grid_on_focus();
}

void *get_mark_grid_info(HWND hwnd)
{
    int i, j;
    for (i=0;i<2;i++)
        for (j=0;j<5;j++)
    {
        if (at_mark_grid[i][j].hwnd == hwnd)
            return &(at_mark_grid[i][j]);

    }


    return NULL;
}

LRESULT CALLBACK mark_grid_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc ;
    PAINTSTRUCT ps ;
    RECT		rect ;
    int cxClient, cyClient;

    t_input_grid_info *pt_grid_info = NULL;
    int row = 0;
    int col = 0;

    POINT point ;

    if (message==WM_PAINT || 
        message==WM_LBUTTONDOWN)
    {
        pt_grid_info = get_mark_grid_info(hwnd);
        row = pt_grid_info->row;
        col = pt_grid_info->col;
    }

    
    switch (message)
    {
        case WM_CREATE:

            return 0 ;

        case WM_PAINT :
        {
            int idx = input_grid_idx(row, col);
            SetRect(&rect, 0, 0, grid_width, grid_height);

            
            hdc = BeginPaint (hwnd, &ps) ;

           if (is_last_input_grid(row, col))
                fw_text_out_middle_trans(hdc
                , &rect, font_size, COLOR_MARK_DIGIT
                , "M", 1);
           else
               fw_text_out_middle_trans(hdc
                , &rect, font_size, COLOR_MARK_DIGIT
                , &mark_str[idx], 1);
            
            EndPaint (hwnd, &ps) ;
            return 0 ;
        }


        case WM_LBUTTONDOWN:
        {
                if (is_last_input_grid(row, col))
                {
                    hide_mark_window();
                    return 0;
                }

                {
                    int idx = input_grid_idx(row, col);
                    if (mark_str[idx]=='.')
                        mark_str[idx] = pt_grid_info->val[0];
                    else
                        mark_str[idx] = '.';

                }
            
            refresh_window(hwnd);

            return 0;
        }

    }



    
    return DefWindowProc(hwnd, message, wParam, lParam) ;
}

int register_mark_grid_win()
{
    WNDCLASS    sub_wndclass;
    sub_wndclass.style      = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    sub_wndclass.lpfnWndProc= mark_grid_WndProc;
    sub_wndclass.cbClsExtra = 0;
    sub_wndclass.cbWndExtra = 0;
    sub_wndclass.hInstance  = g_hInstance;
    sub_wndclass.hIcon      = NULL;
    sub_wndclass.hCursor    = LoadCursor (NULL, IDC_ARROW);
    sub_wndclass.hbrBackground  = CreateSolidBrush(RGB(0x99,0x00,0x00));
    sub_wndclass.lpszMenuName   = NULL;
    sub_wndclass.lpszClassName  = szMarkGridWinClassName;


    if (!RegisterClass (&sub_wndclass))
     {
        MessageBox (NULL, TEXT ("Program requires Windows NT!"),
          szAppName, MB_ICONERROR) ;
        return FAIL;
     }

    return SUCCESS;

}

LRESULT CALLBACK mark_board_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc ;
    PAINTSTRUCT ps ;
    RECT		rect ;
    int cxClient, cyClient;
    int i, j;
    t_grid_info *pt_grid_info = &(at_grid[cur_row][cur_col]);

    POINT point ;


    switch (message)
    {
        case WM_CREATE:
            InitMarkGrids();

            for (i=0;i<2;i++)
                for (j=0;j<5;j++)
            {
                at_mark_grid[i][j].hwnd = CreateWindow (szMarkGridWinClassName, TEXT ("grid"),
                    WS_CHILD,
                    CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
                    hwnd, NULL, g_hInstance, NULL) ;

                ShowWindow (at_mark_grid[i][j].hwnd, 1) ;
                UpdateWindow (at_mark_grid[i][j].hwnd) ;

            }

            return 0 ;


        case WM_SIZE:
        {
            int i, j;
      		cxClient = LOWORD (lParam) ;
      		cyClient = HIWORD (lParam) ;
            grid_width = cxClient/5;
            grid_height= cyClient/2;
            font_size = grid_height-5;
            
            for (i=0;i<2;i++)
                for (j=0;j<5;j++)
            {
                get_input_grid_rect(&rect, i, j);

                MoveWindow(at_mark_grid[i][j].hwnd, 	
                rect.left,  
                rect.top,
                grid_width, 
                grid_height, 
                TRUE) ;

            }


            return 0 ;
        }


    }

    return DefWindowProc(hwnd, message, wParam, lParam) ;
}

int register_mark_win()
{
    WNDCLASS    sub_wndclass;
    sub_wndclass.style      = CS_HREDRAW | CS_VREDRAW;
    sub_wndclass.lpfnWndProc= mark_board_WndProc;
    sub_wndclass.cbClsExtra = 0;
    sub_wndclass.cbWndExtra = 0;
    sub_wndclass.hInstance  = g_hInstance;
    sub_wndclass.hIcon      = NULL;
    sub_wndclass.hCursor    = LoadCursor (NULL, IDC_ARROW);
    sub_wndclass.hbrBackground  = CreateSolidBrush(RGB(0x99,0x00,0x00));
    sub_wndclass.lpszMenuName   = NULL;
    sub_wndclass.lpszClassName  = szMarkBoardWinClassName;


    if (!RegisterClass (&sub_wndclass))
     {
        MessageBox (NULL, TEXT ("Program requires Windows NT!"),
          szAppName, MB_ICONERROR) ;
        return FAIL;
     }

    return SUCCESS;

}

