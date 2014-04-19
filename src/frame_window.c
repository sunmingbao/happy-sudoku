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

TCHAR   szFrameWinClassName[] = TEXT ("frame_win") ;
HWND    hwnd_frame;

HWND hwndTip;

int display_toolbar=1;
int display_statusbar=1;

char doc_file_path[MAX_FILE_PATH_LEN];
char file_to_open[MAX_FILE_PATH_LEN];

void resize_window(HWND hwnd)
{
    RECT rect;
    GetClientRect(hwnd,&rect);
    SendMessage(hwnd, WM_SIZE, SIZE_RESTORED, MAKELPARAM(rect.right, rect.bottom));
}

void set_frame_title(TCHAR *file_name)
{
    TCHAR info[128];
    sprintf(info, TEXT("%s (%s)"), szAppName, file_name);
    SetWindowText(hwnd_frame, info);

}



void open_file()
{
    //if (doc_save_proc()) return;

    load_doc_file(file_to_open);;
    strcpy(doc_file_path, file_to_open);
    set_frame_title(strrchr(doc_file_path, '\\')+1);
    update_file_open_history(doc_file_path);
}


LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc ;
    PAINTSTRUCT ps ;
    static int cxClient, cyClient;
    HMENU hMenu;
    POINT point;
    RECT		rect ;
    int  item_id;
    
    int ret;

    switch (message)
    {
        case WM_CREATE:
            hwnd_frame = hwnd;

            hwndTip =CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,
                        WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP |TTS_BALLOON,
                        CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
                        hwnd,
                        NULL,
                        g_hInstance, 
                        NULL);
            
            CreateToolbar();
            
            hwnd_main_board = CreateWindow (szMainBoardWinClassName, TEXT ("main_board"),
                WS_CHILD,
                CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
                hwnd, NULL, g_hInstance, NULL) ;
            ShowWindow (hwnd_main_board, 1) ;
            UpdateWindow (hwnd_main_board) ;

            CreateStatusBar();

            DragAcceptFiles(hwnd, TRUE);

    set_frame_title(TEXT("点击选中方格, 单击或用键盘输入数字, 右键标记, ↑↓←→格间移动"));

            //ret=get_last_doc_file(file_to_open);

            //if (0==ret) open_file();
            hwnd_tip = CreateWindow (szTipWinClassName, TEXT ("tip win"),
                WS_POPUP,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                hwnd, NULL, g_hInstance, NULL) ;

            ShowWindow (hwnd_tip, 0) ;

            return 0 ;


        case WM_GETMINMAXINFO:
        {
            MINMAXINFO* lpMMI = (void *)lParam;
            lpMMI->ptMinTrackSize.x = 650;
            lpMMI->ptMinTrackSize.y = 600;
            return 0 ;

        }
        
        case WM_SIZE:
        {
            int main_board_x, main_board_y, main_board_size;
            int upper_win_y, upper_win_height;
            
            cxClient = LOWORD (lParam) ;
      		cyClient = HIWORD (lParam) ;

            upper_win_y=display_toolbar?toolbar_height:0;
            upper_win_height=cyClient;
            upper_win_height -= upper_win_y;
            upper_win_height -= (display_statusbar?statusbar_height:0);

             MoveWindow	(	hwnd_toolbar, 	0, 0,
                cxClient, toolbar_height, TRUE) ;

             main_board_size = (upper_win_height>cxClient)?cxClient:upper_win_height;
             main_board_size -= 5;

             MoveWindow	(	hwnd_main_board, 	
                (cxClient-main_board_size)/2,  
                upper_win_y+(upper_win_height-main_board_size)/2,
                main_board_size, 
                main_board_size, 
                TRUE) ;

      	    MoveWindow	(	hwnd_statusbar, 0, cyClient-statusbar_height,
                cxClient, statusbar_height, TRUE) ;

          	return 0 ;
        }

        case WM_LBUTTONDOWN:
        {
            unfocus_any_grid();

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

        case WM_DROPFILES :
            DragQueryFile((HDROP)wParam,
                          0,
                          file_to_open,
                          sizeof(file_to_open));
            DragFinish((HDROP)wParam);
            open_file();
            return 0;

        case WM_INITMENU:

            if (lParam == 0)
            {
                hMenu = GetMenu(hwnd);
                hMenu = GetSubMenu(hMenu, 0);
                hMenu = GetSubMenu(hMenu, 7);
                populate_recent_files(hMenu);
            }

            return 0;

        case 	WM_INITMENUPOPUP:
        {

            if (lParam == 1)
            {
                EnableMenuItem ((HMENU) wParam, IDT_TOOLBAR_UNDO, can_un_do() ? MF_ENABLED : MF_GRAYED);
                EnableMenuItem ((HMENU) wParam, IDT_TOOLBAR_REDO, can_re_do() ? MF_ENABLED : MF_GRAYED);

                return 0;

            }

            break;
        }

        case 	WM_COMMAND:
            hMenu = GetMenu (hwnd) ;
            item_id = LOWORD(wParam);

            if (item_id>=ID_STAGE_1 && item_id < ID_STAGE_1+MAX_STAGE_NUM)
            {

                select_stage(item_id - ID_STAGE_1 + 1);

                return 0 ;

            }
            switch (item_id)
            {
                case    IDM_APP_ABOUT:
                DialogBox (g_hInstance, TEXT("ABOUTBOX"), hwnd, AboutDlgProc) ;
               	return 0 ;

                case    IDM_GET_NEW_VERSION:
                ShellExecute(NULL, "open"
                    , "http://sourceforge.net/projects/happy-sudoku/files/latest/download?source=directory"
                    , NULL, NULL, SW_SHOWNORMAL);

               	return 0 ;

                case    IDM_GET_SOURCE:
                ShellExecute(NULL, "open"
                    , "https://sourceforge.net/projects/happy-sudoku/files/v2.x/"
                    , NULL, NULL, SW_SHOWNORMAL);

               	return 0 ;

                case    IDM_OPEN_OFFICIAL_SITE:
                ShellExecute(NULL, "open"
                    , "http://sunmingbao.freevar.com/"
                    , NULL, NULL, SW_SHOWNORMAL);

               	return 0 ;

                case    IDM_APP_EXIT:
       				SendMessage (hwnd, WM_CLOSE, 0, 0) ;
       				return 0 ;

                case    IDM_APP_TOOLBAR_WIN:
                    if (display_toolbar)
                    {
                        display_toolbar = 0;
           				ShowWindow(hwnd_toolbar, 0);
                        UpdateWindow (hwnd_frame) ;
                        CheckMenuItem (hMenu, item_id, MF_UNCHECKED) ;

                    }
                    else
                    {
                        display_toolbar= 1;
           				ShowWindow(hwnd_toolbar, 1);
                        UpdateWindow (hwnd_frame) ;
                        CheckMenuItem (hMenu, item_id, MF_CHECKED) ;
                    }
                    resize_window(hwnd_frame);
       				return 0 ;

                case    IDM_APP_STATUSBAR_WIN:
                    if (display_statusbar)
                    {
                        display_statusbar = 0;
           				ShowWindow(hwnd_statusbar, 0);
                        UpdateWindow (hwnd_frame) ;
                        CheckMenuItem (hMenu, item_id, MF_UNCHECKED) ;

                    }
                    else
                    {
                        display_statusbar= 1;
           				ShowWindow(hwnd_statusbar, 1);
                        UpdateWindow (hwnd_frame) ;
                        CheckMenuItem (hMenu, item_id, MF_CHECKED) ;
                    }
                    resize_window(hwnd_frame);
       				return 0 ;


                case    IDT_TOOLBAR_UNDO:
                    un_do();
                    if (!can_un_do())
                        set_toolbar_button(IDT_TOOLBAR_UNDO, 0);

               	    return 0 ;

                case    IDT_TOOLBAR_REDO:
                    re_do();
                    if (!can_re_do())
                        set_toolbar_button(IDT_TOOLBAR_REDO, 0);
       				return 0 ;

                case    IDT_TOOLBAR_HINT:
                {
                    if (display_help != 2)
                        display_help=2;
                    else
                        display_help=0;

                    SendMessage(hwnd_toolbar, TB_CHECKBUTTON, IDT_TOOLBAR_HINT, (2==display_help));
                    CheckMenuItem(hMenu, IDT_TOOLBAR_HINT, (2==display_help)?MF_CHECKED:MF_UNCHECKED);
                    SendMessage(hwnd_toolbar, TB_CHECKBUTTON, IDT_TOOLBAR_MARK, (1==display_help));
                    CheckMenuItem(hMenu, IDT_TOOLBAR_MARK, (1==display_help)?MF_CHECKED:MF_UNCHECKED);

                    InvalidateRect(hwnd_main_board, NULL, TRUE);


       				return 0 ;
                }

                case    IDT_TOOLBAR_MARK:
                {
                    if (display_help != 1)
                        display_help=1;
                    else
                        display_help=0;

                    SendMessage(hwnd_toolbar, TB_CHECKBUTTON, IDT_TOOLBAR_HINT, (2==display_help));
                    CheckMenuItem(hMenu, IDT_TOOLBAR_HINT, (2==display_help)?MF_CHECKED:MF_UNCHECKED);
                    SendMessage(hwnd_toolbar, TB_CHECKBUTTON, IDT_TOOLBAR_MARK, (1==display_help));
                    CheckMenuItem(hMenu, IDT_TOOLBAR_MARK, (1==display_help)?MF_CHECKED:MF_UNCHECKED);

                    InvalidateRect(hwnd_main_board, NULL, TRUE);


       				return 0 ;
                }

                case    IDM_SOUND:
                {
                    if (is_sound_enabled())
                        disable_sound();
                    else
                        enable_sound();

                    CheckMenuItem(hMenu, item_id, (is_sound_enabled())?MF_CHECKED:MF_UNCHECKED);

       				return 0 ;
                }

                case    IDM_MUSIC:
                {
                    if (play_music)
                    {
                        play_music = 0;
                        mciSendString("stop music", NULL, 0, 0);
                    }
                    else
                    {
                        play_music = 1;
                        mciSendString("play music repeat", NULL, 0, 0);
                    }

                    CheckMenuItem(hMenu, item_id, (play_music)?MF_CHECKED:MF_UNCHECKED);

       				return 0 ;
                }

                case    IDT_TOOLBAR_TIMER:
                {
                    display_time = !display_time;
                    SendMessage(hwnd_toolbar, TB_CHECKBUTTON, item_id, display_time);
                    CheckMenuItem(hMenu, item_id, display_time?MF_CHECKED:MF_UNCHECKED);
                    update_statusbar_time();
       		        return 0 ;
                }
                
                case    IDM_ARCH_SAVE:
                {
                        ret=get_save_file_name(file_to_open, hwnd, DOC_FILE_FILTER, DOC_FILE_SUFFIX);
                        if (ret) return 0 ;
                        SaveAsArch(file_to_open);
                       	return 0 ;
                }
                
                case    IDM_SAVE_AS_PUZZLE:
                {
                        ret=get_save_file_name(file_to_open, hwnd, "数独迷题(*.""sdpzl"")\0*.""sdpzl""\0\0", "sdpzl");
                        if (ret) return 0 ;
                        SaveAsPuzzle(file_to_open);
                       	return 0 ;
                }

                case    IDM_CGMS:
                {
                    stage_mode_begin();
                    return 0 ;
                }

                case    IDM_EMPTY_GAME:
                {
                    select_empty_game();
                    return 0 ;
                }

                case    IDM_ARCH_OPEN:
                {
                    if (get_open_file_name(file_to_open, hwnd, DOC_FILE_FILTER))
                        return 0;
                    LoadArch(file_to_open);
                   	return 0 ;
                }

                case    IDM_RAND_GAME:
                {
                    LoadRandGame();
                   	return 0 ;
                }

                case    IDM_OPEN_PUZZLE:
                {
                    if (get_open_file_name(file_to_open, hwnd, "数独迷题(*.""sdpzl"")\0*.""sdpzl""\0\0"))
                        return 0;
                    load_puzzle_as_game(file_to_open);
                   	return 0 ;
                }

                case    IDM_SUDOKU_SOLVER:
                {
                    resolve_puzzle(".57.6........94..5.13....2......3..2..85.13..1..6..57..2....86.3..81..578...4.93.");
                   	return 0 ;
                }

                case    ID_FILE_RECENT_FILE_BEGIN+0:
                case    ID_FILE_RECENT_FILE_BEGIN+1:
                case    ID_FILE_RECENT_FILE_BEGIN+2:
                case    ID_FILE_RECENT_FILE_BEGIN+3:
                case    ID_FILE_RECENT_FILE_BEGIN+4:
                case    ID_FILE_RECENT_FILE_BEGIN+5:
                case    ID_FILE_RECENT_FILE_BEGIN+6:
                case    ID_FILE_RECENT_FILE_BEGIN+7:
                case    ID_FILE_RECENT_FILE_BEGIN+8:
                case    ID_FILE_RECENT_FILE_BEGIN+9:
                {
                    get_file_path_by_idx(file_to_open
                        , item_id - ID_FILE_RECENT_FILE_BEGIN);

                    if (file_exists(file_to_open)) LoadArch(file_to_open);
                    return 0 ;

                }


            }

            return 0;

     	case	WM_CLOSE:
            //if (doc_save_proc()) return 0;
            DestroyWindow (hwnd) ;
            return 0 ;

        case    WM_DESTROY :
            PostQuitMessage (0) ;
            return 0 ;
    }
    return DefWindowProc (hwnd, message, wParam, lParam) ;
}

int register_frame()
{
    WNDCLASS    wndclass;
    wndclass.style      = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc= WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance  = g_hInstance;
    wndclass.hIcon      = LoadIcon (g_hInstance, TEXT("app_icon"));

    wndclass.hCursor      = LoadCursor (NULL, IDC_ARROW);

    wndclass.hbrBackground  = (HBRUSH)GetStockObject(GRAY_BRUSH);
    wndclass.lpszMenuName   = TEXT("MY_MENU");
    wndclass.lpszClassName  = szFrameWinClassName;

    if (!RegisterClass (&wndclass))
     {
        MessageBox (NULL, TEXT("Register frame window class failed.\r\nProgram requires Windows NT!"),
          szAppName, MB_ICONERROR) ;
        return FAIL;
     }

    return SUCCESS;

}


int create_windows(int iCmdShow)
{
    hwnd_frame = CreateWindow (szFrameWinClassName, szAppName,
            WS_OVERLAPPEDWINDOW|WS_MAXIMIZE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
            CW_USEDEFAULT, CW_USEDEFAULT,
            CW_USEDEFAULT, CW_USEDEFAULT,
            NULL, NULL, g_hInstance, NULL) ;

    
    ShowWindow (hwnd_frame, SW_MAXIMIZE) ;
    UpdateWindow (hwnd_frame) ;

    return 0;

}




