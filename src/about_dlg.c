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

#include "res.h"


BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT message,WPARAM wParam, LPARAM lParam)
{
    TCHAR info[1024];

    sprintf(info
        , "%s    V%c.%c.%c\r\n"
        "(����ʱ��: %s) \r\n\r\n"
        "���ߣ�%s (���� %s)\r\n\r\n"
        "======================\r\n\r\n"
        "�����Ϊ��ѡ���Դ�����\r\n"
        "������İ�Ȩ(����Դ�뼰�����Ʒ����汾)��һ�й������С�\r\n"
        "����������ʹ�á������������\r\n"
        "��Ҳ�������κ���ʽ���κ�Ŀ��ʹ�ñ����(����Դ�뼰�����Ʒ����汾)���������κΰ�Ȩ���ơ�\r\n\r\n"
        "======================\r\n\r\n"
        
        "���ڱ�����������κ�������飬��ӭ��ϵ���ߡ�\r\n"
        "����:%s\r\n"
        "%s"
        , szAppName
        , version[0] , version[1] , version[2]
        , BUILD_TIME
        , szAuthor, szCompany
        , szEmail
        , szContact);
     	switch (message)
     	{
     	case 	WM_INITDIALOG :
                center_child_win(hwnd_frame, hDlg);
                SetDlgItemText(hDlg, ID_ABOUT_INFO, info);
                SetFocus(GetDlgItem(hDlg, IDOK));
          		return FALSE ;

        case 	WM_CLOSE:
   				SendMessage(hDlg, WM_COMMAND, IDCANCEL, 0);
   				return TRUE ;

     	case 	WM_COMMAND :
          		switch (LOWORD (wParam))
          		{

                case 	ID_AB_SOURCE:
               		SendMessage(hwnd_frame, WM_COMMAND, IDM_GET_SOURCE, 0);
               		return TRUE ;
               		
               	case 	ID_AB_SITE:
               		SendMessage(hwnd_frame, WM_COMMAND, IDM_OPEN_OFFICIAL_SITE, 0);
               		return TRUE ;
               		
               	case 	ID_AB_UPDATE:
               		SendMessage(hwnd_frame, WM_COMMAND, IDM_GET_NEW_VERSION, 0);
               		return TRUE ;
        
          		case 	IDOK :
          		case 	IDCANCEL :
               				EndDialog (hDlg, 0) ;
               				return TRUE ;
          }
          		break ;
     }
  	return FALSE ;
}



