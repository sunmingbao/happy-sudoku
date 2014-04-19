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
#include "app_property.h"
#include "common.h"
#include "utils.h"
#include "global_symbols.h"
#include "sudoku.h"
#include "res.h"

FILE *fp_solve_result;
uint64_t result_num;
static char puzzle_to_resolve[82]; 
static char puzzle_to_resolve_hm[256]; 
HWND hDlg_resolve_puzzle_wait;

static void for_each_result(uint64_t result_idx, t_board *ptBoard)
{
    char buf[128];
    char buf_2[128]={0};
    int i;

    result_num++;

    sprintf(buf, "第 %d 个解\n", (int)result_idx);
    fputs(buf, fp_solve_result);

    board_to_input_str(buf, ptBoard);

    for (i=0; i<MAX_ROW_NUM; i++)
    {
        memcpy(buf_2, buf+i*9, 9);
        fputs(buf_2, fp_solve_result);
        fputs("\n", fp_solve_result);
    }

    fputs("\n", fp_solve_result);

    set_int_text(GetDlgItem(hDlg_resolve_puzzle_wait, IDC_VAR_INFO), (int)result_idx);
}

DWORD WINAPI  do_resolve_puzzle(LPVOID lpParameter)
{
    char buf[512];
    result_num = 0;
    fp_solve_result = fopen("result.txt", "w");
    sprintf(buf, "全部解的个数: %-22d\n\n", 0);
    fputs(buf, fp_solve_result);

    solve(puzzle_to_resolve, 0, for_each_result);

    sprintf(buf, "全部解的个数: %-22d", (int)result_num);
    rewind(fp_solve_result);
    fputs(buf, fp_solve_result);

    fclose(fp_solve_result);
    SendMessage(hDlg_resolve_puzzle_wait, WM_COMMAND, IDOK, 0);

}

BOOL CALLBACK ResolvePuzzleWaitDlgProc(HWND hDlg, UINT message,WPARAM wParam, LPARAM lParam)
{

 	switch (message)
 	{
         	case 	WM_INITDIALOG :
                    hDlg_resolve_puzzle_wait = hDlg;
                    center_child_win(hwnd_frame, hDlg);
                    SetWindowText(hDlg, "求解迷题");
                    SetDlgItemText(hDlg, IDC_FIXED_INFO, "已求得解的数目: ");
                    set_int_text(GetDlgItem(hDlg_resolve_puzzle_wait, IDC_VAR_INFO), 0);
                    launch_thread(do_resolve_puzzle, NULL);
              		return FALSE ;


            case 	WM_CLOSE:
       				SendMessage(hDlg, WM_COMMAND, IDCANCEL, 0);
       				return TRUE ;

         	case 	WM_COMMAND :
      		switch (LOWORD (wParam))
      		{

              		case 	IDOK :
                        EndDialog (hDlg, IDOK) ;
           				return TRUE ;

              		case 	IDCANCEL :
                        stop_resolve();
           				EndDialog (hDlg, IDCANCEL) ;
           				return TRUE ;
            }
            
            break ;
     }
  	return FALSE ;
}

void resolve_puzzle(const char *input)
{
    strcpy(puzzle_to_resolve, input);
    DialogBox(g_hInstance, TEXT("GEN_PUZZLE_WAIT_DLG"), hwnd_frame, ResolvePuzzleWaitDlgProc);

    if (result_num>0)
    ShellExecute(NULL, "open"
                    , "result.txt"
                    , NULL, NULL, SW_SHOWMAXIMIZED);

}

BOOL CALLBACK PuzzleInputDlgProc(HWND hDlg, UINT message,WPARAM wParam, LPARAM lParam)
{
    char buf[1024];
    char puzzle_file_path[MAX_FILE_PATH_LEN];
    FILE *fp;

    switch (message)
 	{
         	case 	WM_INITDIALOG :
                    center_child_win(hwnd_frame, hDlg);
                    SendMessage(GetDlgItem(hDlg, ID_PUZZLE_INPUT), WM_SETFONT, (WPARAM)GetStockObject(SYSTEM_FIXED_FONT), 0);
                    SetDlgItemText(hDlg, ID_PUZZLE_INPUT, puzzle_to_resolve_hm);
              		return FALSE ;


            case 	WM_CLOSE:
       				SendMessage(hDlg, WM_COMMAND, IDCANCEL, 0);
       				return TRUE ;

         	case 	WM_COMMAND :
      		switch (LOWORD (wParam))
      		{
      		        case    ID_IMPORT_FROM_FILE:
                    {
                        if (get_open_file_name(puzzle_file_path, hDlg, "数独迷题(*.""sdpzl"")\0*.""sdpzl""\0\0"))
                        return 0;

                        fp=fopen(puzzle_file_path,"r");
                        fread(buf, 1, sizeof(buf),  fp );
                        fclose(fp);
                        SetDlgItemText(hDlg, ID_PUZZLE_INPUT, buf);
                        return TRUE ;


                    }

              		case 	IDOK :

                        GetDlgItemText(hDlg, ID_PUZZLE_INPUT, buf, sizeof(buf));

                        if (!format_input(puzzle_to_resolve, buf))
                        {
                            WinPrintf(hDlg, "错误", "迷题输入非法");
                            return TRUE ;
                        }
                        
                        EndDialog (hDlg, IDOK) ;
           				return TRUE ;

              		case 	IDCANCEL :
           				EndDialog (hDlg, IDCANCEL) ;
           				return TRUE ;
            }
            
            break ;
     }
  	return FALSE ;
}


void get_puzzle_and_resolve(char *input)
{
    int ret;

    strcpy(puzzle_to_resolve_hm, input);
    ret = DialogBox(g_hInstance, TEXT("PUZZLE_INPUT_DLG"), hwnd_frame, PuzzleInputDlgProc);

    if (IDCANCEL==ret) return;
    
    resolve_puzzle(puzzle_to_resolve);

}
