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
#include "common.h"
#include "utils.h"
#include "global_symbols.h"
#include "res.h"

HWND    hwnd_toolbar;
int     toolbar_height;

TBBUTTON at_button[] =                
    {
        {I_IMAGENONE , -1, TBSTATE_ENABLED, TBSTYLE_SEP|TBSTYLE_AUTOSIZE},
        {I_IMAGENONE , -1, TBSTATE_ENABLED, TBSTYLE_SEP|TBSTYLE_AUTOSIZE},
        {I_IMAGENONE , -1, TBSTATE_ENABLED, TBSTYLE_SEP|TBSTYLE_AUTOSIZE},
        {I_IMAGENONE , -1, TBSTATE_ENABLED, TBSTYLE_SEP|TBSTYLE_AUTOSIZE},
        {2, IDT_TOOLBAR_HINT, TBSTATE_ENABLED, TBSTYLE_AUTOSIZE , {0}, 0, (int)"��ʾ��ʾ(CTRL+H)" },
        {3, IDT_TOOLBAR_MARK, TBSTATE_ENABLED, TBSTYLE_AUTOSIZE , {0}, 0, (int)"��ʾ���(CTRL+M)" },
        {I_IMAGENONE , -1, TBSTATE_ENABLED, TBSTYLE_SEP|TBSTYLE_AUTOSIZE},
        {4, IDT_TOOLBAR_TIMER, TBSTATE_ENABLED, TBSTYLE_AUTOSIZE , {0}, 0, (int)"��ʾ��ʱ(CTRL+T)" },
        {I_IMAGENONE , -1, TBSTATE_ENABLED, TBSTYLE_SEP|TBSTYLE_AUTOSIZE},
        {I_IMAGENONE , -1, TBSTATE_ENABLED, TBSTYLE_SEP|TBSTYLE_AUTOSIZE},
        {0, IDT_TOOLBAR_UNDO, TBSTATE_INDETERMINATE, TBSTYLE_AUTOSIZE , {0}, 0, (int)"����(CTRL+U)"},
        {1, IDT_TOOLBAR_REDO, TBSTATE_INDETERMINATE, TBSTYLE_AUTOSIZE , {0}, 0, (int)"ǰ��(CTRL+Y)"},

    };

HBITMAP hbmpToolbar[3];
HIMAGELIST g_hImageList;

WNDPROC old_tb_proc;
LRESULT CALLBACK my_tb_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
 	int cxClient, cyClient;
    RECT  rc;
    int button_width, button_height;
 	switch (message)
 	{
     	case WM_SIZE :
            cxClient = LOWORD (lParam) ;
      		cyClient = HIWORD (lParam) ;

            /* �������ؼ���С��λ�õ��� */
            /* �Ȼ�ȡ���ұߵİ�Ŧ�����굽rc�С�
               Ȼ����ݴ�����������ؼ���λ�� */
            SendMessage(hwnd_toolbar,TB_GETITEMRECT,(WPARAM)ARRAY_SIZE(at_button)-1,(LPARAM)&rc);
            button_width = rc.right-rc.left;
            button_height = rc.bottom-rc.top;

            break;


        case 	WM_COMMAND:
            /* �������ϵĿؼ���Ϣ���� */
            break;

 	}

    return CallWindowProc (old_tb_proc, hwnd, message, wParam,lParam) ;
}

int CreateToolbar()
{

BITMAP BMPInfo;
HIMAGELIST h0, h1;
HICON hiconItem; 

//����������
    hwnd_toolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL
    , WS_CHILD | WS_VISIBLE | WS_BORDER |  TBSTYLE_TOOLTIPS | TBSTYLE_LIST  
    , 0, 0, 0, 0,
        hwnd_frame, (HMENU)NULL, g_hInstance, NULL);
    SendMessage(hwnd_toolbar, TB_SETEXTENDEDSTYLE, 0,  TBSTYLE_EX_MIXEDBUTTONS );

    if(hwnd_toolbar == NULL)
    {
        err_info_box(NULL, TEXT("����������ʧ��"));
        return FAIL;
     }
    
//���ذ�Ŧͼ��
    h0 = ImageList_Create(32,32,ILC_MASK|ILC_COLORDDB,8,0);

    hiconItem = LoadIcon(g_hInstance, TEXT("tb_icon_undo"));
    ImageList_AddIcon(h0, hiconItem);
    DestroyIcon(hiconItem);
    
    hiconItem = LoadIcon(g_hInstance, TEXT("tb_icon_redo"));
    ImageList_AddIcon(h0, hiconItem);
    DestroyIcon(hiconItem);
    

    hiconItem = LoadIcon(g_hInstance, TEXT("tb_icon_hint"));
    ImageList_AddIcon(h0, hiconItem);
    DestroyIcon(hiconItem);

    hiconItem = LoadIcon(g_hInstance, TEXT("tb_icon_mark"));
    ImageList_AddIcon(h0, hiconItem);
    DestroyIcon(hiconItem);

    hiconItem = LoadIcon(g_hInstance, TEXT("tb_icon_timer"));
    ImageList_AddIcon(h0, hiconItem);
    DestroyIcon(hiconItem);

    //���ù�������Ŧ����״̬��ͼ��
    SendMessage(hwnd_toolbar,TB_SETIMAGELIST,0,(LPARAM)h0);

    h1 = ImageList_Create(32,32,ILC_MASK|ILC_COLORDDB,8,0);

    hiconItem = LoadIcon(g_hInstance, TEXT("tb_icon_undo_dis"));
    ImageList_AddIcon(h1, hiconItem);
    DestroyIcon(hiconItem);
    
    hiconItem = LoadIcon(g_hInstance, TEXT("tb_icon_redo_dis"));
    ImageList_AddIcon(h1, hiconItem);
    DestroyIcon(hiconItem);
    

    hiconItem = LoadIcon(g_hInstance, TEXT("tb_icon_hint"));
    ImageList_AddIcon(h1, hiconItem);
    DestroyIcon(hiconItem);

    hiconItem = LoadIcon(g_hInstance, TEXT("tb_icon_mark"));
    ImageList_AddIcon(h1, hiconItem);
    DestroyIcon(hiconItem);

    hiconItem = LoadIcon(g_hInstance, TEXT("tb_icon_timer"));
    ImageList_AddIcon(h1, hiconItem);
    DestroyIcon(hiconItem);

    //���ù�������Ŧ������״̬��ͼ��
    SendMessage(hwnd_toolbar,TB_SETDISABLEDIMAGELIST,0,(LPARAM)h1);

    //����ť�빤��������
    SendMessage(hwnd_toolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
    SendMessage(hwnd_toolbar, TB_ADDBUTTONS, ARRAY_SIZE(at_button), (LPARAM)at_button);
    SendMessage(hwnd_toolbar,TB_SETMAXTEXTROWS, 0, 0);
    
    // Resize the toolbar, and then show it.
    SendMessage(hwnd_toolbar, TB_AUTOSIZE, 0, 0); 
    SendMessage(hwnd_toolbar, TB_CHECKBUTTON, IDT_TOOLBAR_MARK, 1);
    SendMessage(hwnd_toolbar, TB_CHECKBUTTON, IDT_TOOLBAR_TIMER, 1);
    ShowWindow(hwnd_toolbar,  TRUE);
    toolbar_height = win_height(hwnd_toolbar);

    

    {
        /* �ڹ������ϴ�������Ŀؼ�����������checkbox��... */

        /* �����������໯���Ա��ڴ���ؼ���Ϣ */
        old_tb_proc = (WNDPROC) SetWindowLong (hwnd_toolbar, 
                                     	GWL_WNDPROC, (LONG) my_tb_proc) ;
    }

    return SUCCESS;
}

void set_toolbar_button(int id, int is_enabled)
{
    SendMessage(hwnd_toolbar, TB_SETSTATE, id,  is_enabled?TBSTATE_ENABLED:TBSTATE_INDETERMINATE); 
}

