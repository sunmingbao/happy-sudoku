/* 
 * �����Ϊ��ѡ���Դ�����
 * ������İ�Ȩ(����Դ�뼰�����Ʒ����汾)��һ�й������С�
 * ����������ʹ�á������������
 * ��Ҳ�������κ���ʽ���κ�Ŀ��ʹ�ñ����(����Դ�뼰�����Ʒ����汾)���������κΰ�Ȩ���ơ�
 * =====================
 * ����: ������
 * ����: sunmingbao@126.com
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
HWND hDlg_resolve_puzzle_wait;

static void for_each_result(uint64_t result_idx, t_board *ptBoard)
{
    char buf[128];
    char buf_2[128]={0};
    int i;

    result_num++;

    sprintf(buf, "�� %d ����\n", (int)result_idx);
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
    sprintf(buf, "ȫ����ĸ���: %-22d\n\n", 0);
    fputs(buf, fp_solve_result);

    solve(puzzle_to_resolve, 0, for_each_result);

    sprintf(buf, "ȫ����ĸ���: %-22d", (int)result_num);
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
                    SetWindowText(hDlg, "�������");
                    SetDlgItemText(hDlg, IDC_FIXED_INFO, "����ý����Ŀ: ");
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
                    , NULL, NULL, SW_SHOWNORMAL);

}


void get_puzzle_and_resolve()
{
    int ret = DialogBox(g_hInstance, TEXT("GEN_PUZZLE_WAIT_DLG"), hwnd_frame, ResolvePuzzleWaitDlgProc);

    if (IDCANCEL==ret) return;
    
    resolve_puzzle("");

}