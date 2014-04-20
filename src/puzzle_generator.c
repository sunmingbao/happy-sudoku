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
#include "ext_utils_textout.h"

void generate_digit_sequence(char *output)
{
    /* 这里不需要乱序。
    因为generate_pos_sequence摇出的位置集是乱序的。*/
    strcpy(output, "123456789");
}


t_pos_set generate_pos_sequence()
{
    t_pos_set  pos_set = {0};
    int row, col;
    t_pos  pos;

    while (pos_set.num < 9)
    {
        row = rand_range(0, 8);
        col = rand_range(0, 8);
        pos = make_pos(row, col);
        if (pos_set_has_pos(&pos_set, &pos))
        {
            //DbgPrintf("qqq %d %d", pos.row, pos.col);
            continue;
        }
        
        add_pos(&pos_set, pos);
       
    }

    return pos_set;
}


void generate_init_input(char *output)
{
    int i;
    t_pos pos;
    char buf[10];
    t_pos_set  pos_set;

    generate_digit_sequence(buf);
    pos_set = generate_pos_sequence();
    //DbgPrintf("generate_pos_sequence");
    strcpy(output, empty_stage_str);
    for (i=0;i<pos_set.num; i++)
    {
        pos = pos_set.at_pos[i];
        output[pos.row*9 + pos.col] =  buf[i];
    }
}

char generated_input[128];
static void for_each_result(uint64_t result_idx, t_board *ptBoard)
{
    board_to_input_str(generated_input, ptBoard);
}

static int  init_grid_num;
void del_some_inputs()
{
    int del_num = 81 - init_grid_num;
    int idx;

    while (del_num)
    {
        idx = rand_range(0, 80);
        if (isdigit(generated_input[idx]))
        {
            generated_input[idx] = '.';
            del_num--;
        }
    }
    
}

int stop_gen_puzzle;
HWND hDlg_gen_puzzle_wait;
DWORD WINAPI  do_generate_puzzle(LPVOID lpParameter)
{
    uint64_t result_num = 0;
    char buf[128];

    stop_gen_puzzle = 0;
    srand((unsigned)time(NULL));

    while (0==result_num)
    {
        if (stop_gen_puzzle)
        {
            return 0;
        }
        
        //DbgPrintf("generate_init_input");

        generate_init_input(buf);
        result_num = solve(buf, rand_range(1, 256), for_each_result);
    }

    del_some_inputs();
    SendMessage(hDlg_gen_puzzle_wait, WM_COMMAND, IDOK, 0);
    return 0;
}

BOOL CALLBACK GenPuzzleWaitDlgProc(HWND hDlg, UINT message,WPARAM wParam, LPARAM lParam)
{
    static int timer_msg_cnt = 0;
    char dots[8];

 	switch (message)
 	{
         	case 	WM_INITDIALOG :
                    hDlg_gen_puzzle_wait = hDlg;
                    center_child_win(hwnd_frame, hDlg);
                    SetWindowText(hDlg, "生成迷题");
                    SendMessage(GetDlgItem(hDlg, IDC_FIXED_INFO), WM_SETFONT, (WPARAM)h_font_32_px, 0);
                    SendMessage(GetDlgItem(hDlg, IDC_VAR_INFO), WM_SETFONT, (WPARAM)h_font_32_px, 0);

                    SetDlgItemText(hDlg, IDC_FIXED_INFO, "正在生成迷题，请稍后");
                    SetTimer(hDlg, TIMER_GEN_PUZZLE_WAIT, TIMER_GEN_PUZZLE_WAIT_GAP, NULL);
                    launch_thread(do_generate_puzzle, NULL);
              		return FALSE ;

            case WM_TIMER:
            {
                timer_msg_cnt = (timer_msg_cnt+1)%7;
                memcpy(dots, "......", timer_msg_cnt);
                dots[timer_msg_cnt] = 0;
                SetDlgItemText(hDlg, IDC_VAR_INFO, dots);
                
                
                return TRUE ;
            }

            case 	WM_CLOSE:
       				SendMessage(hDlg, WM_COMMAND, IDCANCEL, 0);
       				return TRUE ;

         	case 	WM_COMMAND :
      		switch (LOWORD (wParam))
      		{

              		case 	IDOK :
                        KillTimer (hDlg, TIMER_GEN_PUZZLE_WAIT);
                        EndDialog (hDlg, IDOK) ;
           				return TRUE ;

              		case 	IDCANCEL :
                        stop_gen_puzzle = 1;
                        stop_resolve();
                        KillTimer (hDlg, TIMER_GEN_PUZZLE_WAIT);
           				EndDialog (hDlg, IDCANCEL) ;
           				return TRUE ;
            }
            
            break ;
     }
  	return FALSE ;
}

int generate_puzzle(char *output)
{
    int ret;

    ret = select_int_value("预置数字格子数", "", 1, 78, &init_grid_num);

    if (IDOK!=ret) return ret;

    ret = DialogBox(g_hInstance, TEXT("GEN_PUZZLE_WAIT_DLG"), hwnd_frame, GenPuzzleWaitDlgProc);

    if (IDOK==ret)
        strcpy(output, generated_input);

    return ret;
}

