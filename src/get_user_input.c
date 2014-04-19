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

static int g_min, g_max, *gp_result;
char *g_cap;
char *g_tip;

void clear_comb(HWND hwndCtl)
{
    while(ComboBox_GetCount(hwndCtl)>0)
    {
        ComboBox_DeleteString(hwndCtl, 0);
    }
}

void init_int_list_comb(HWND comb)
{
    int i;
    char info[64], info_2[32];

    clear_comb(comb);
    for (i=g_min;i<=g_max;i++)
    {
        sprintf(info, "%d", i);
        SendMessage(comb,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)info);
    }

}

BOOL CALLBACK IntSelectDlgProc(HWND hDlg, UINT message,WPARAM wParam, LPARAM lParam)
{
    int ret;

    switch (message)
 	{
         	case 	WM_INITDIALOG :
                    center_child_win(hwnd_frame, hDlg);
                    SendMessage(GetDlgItem(hDlg, IDC_FIXED_INFO), WM_SETFONT, (WPARAM)GetStockObject(SYSTEM_FIXED_FONT), 0);
                    SetWindowText(hDlg, g_cap);
                    SetDlgItemText(hDlg, IDC_FIXED_INFO, g_tip);
                    init_int_list_comb(GetDlgItem(hDlg, ID_INT_LIST_COMB));
                    SendMessage(GetDlgItem(hDlg, ID_INT_LIST_COMB), CB_SETCURSEL, 39, (LPARAM)0);
              		return FALSE ;


            case 	WM_CLOSE:
       				SendMessage(hDlg, WM_COMMAND, IDCANCEL, 0);
       				return TRUE ;

         	case 	WM_COMMAND :
      		switch (LOWORD (wParam))
      		{

              		case 	IDOK :
                        ret = SendMessage(GetDlgItem(hDlg, ID_INT_LIST_COMB)
             , (UINT) CB_GETCURSEL, (WPARAM) 0, (LPARAM) 0);
                        EndDialog (hDlg, IDOK) ;
                        
                        *gp_result = ret + 1;
           				return TRUE ;

              		case 	IDCANCEL :
           				EndDialog (hDlg, IDCANCEL) ;
           				return TRUE ;
            }
            
            break ;
     }
  	return FALSE ;
}

int select_int_value(char *CAP, char *tip, int min, int max, int *presult)
{
    int ret;

    g_cap = CAP;
    g_tip = tip;
    g_min = min;
    g_max = max;
    gp_result = presult;

    ret = DialogBox(g_hInstance, TEXT("INT_SELECT_DLG"), hwnd_frame, IntSelectDlgProc);

    return ret;
}
