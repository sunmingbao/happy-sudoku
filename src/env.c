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
    init_gui_info();

    CreateDirectory("C:\\WINDOWS\\temp", NULL);
    GetModuleFileName(NULL,  exe_file_path,  sizeof(exe_file_path));
    pchar=strrchr(exe_file_path, '\\');
    *pchar=0;
    SetCurrentDirectory(exe_file_path);

    reg_sys_win_classes();

    srand((unsigned)time(NULL));

}


