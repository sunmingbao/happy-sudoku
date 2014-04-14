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
#include <tchar.h> 
#include <commctrl.h>
#include <windowsx.h>
#include "defs.h"

int scrn_width;  /* ��Ļ��� */
int scrn_height; /* ��Ļ�߶� */

int cxChar; /* SYSTEM_FIXED_FONT width */
int cyChar; /* SYSTEM_FIXED_FONT height */

void init_char_size_info()
{
    HDC hdc = CreateIC (TEXT ("DISPLAY"), NULL, NULL, NULL) ;
    TEXTMETRIC textmetric;

    SelectObject (hdc, GetStockObject(SYSTEM_FIXED_FONT)) ;
    GetTextMetrics(hdc, &textmetric) ;
    DeleteObject (SelectObject (hdc, GetStockObject(SYSTEM_FONT))) ;
    DeleteDC (hdc) ;

    cxChar = textmetric.tmAveCharWidth ;
    cyChar = textmetric.tmHeight + textmetric.tmExternalLeading ;

}

int reg_sys_win_classes()
{
    INITCOMMONCONTROLSEX icex;           // Structure for control initialization.
    icex.dwSize = sizeof(icex);
    icex.dwICC = ICC_STANDARD_CLASSES|ICC_BAR_CLASSES|ICC_TAB_CLASSES
        |ICC_TREEVIEW_CLASSES|ICC_LISTVIEW_CLASSES;


    InitCommonControlsEx(&icex);
    InitCommonControls();

}

void env_init()
{
    char exe_file_path[MAX_FILE_PATH_LEN];
    char *pchar;
    init_char_size_info();
    CreateDirectory("C:\\WINDOWS\\temp", NULL);
    GetModuleFileName(NULL,  exe_file_path,  sizeof(exe_file_path));
    pchar=strrchr(exe_file_path, '\\');
    *pchar=0;
    SetCurrentDirectory(exe_file_path);

    scrn_width = GetSystemMetrics(SM_CXSCREEN);
    scrn_height = GetSystemMetrics(SM_CYSCREEN);

    reg_sys_win_classes();
}


