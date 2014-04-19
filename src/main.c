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
#include "env.h"
#include "global_symbols.h"


HINSTANCE g_hInstance;
int       init_over;



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow)
{
    MSG     msg ;
    HACCEL  hAccel ;

    g_hInstance = hInstance;

    env_init();

    register_frame();
    register_main_board_win();
    register_grid_win();
    register_input_board_win();
    register_input_grid_win();
    register_tip_win();

    create_app_fonts();
    create_windows(iCmdShow);

 	hAccel = LoadAccelerators (hInstance, TEXT("MY_ACCELER")) ;

    /* ��ʼ����ϡ�*/
    init_over=1;

    /* ���������Ϣѭ�� */
    
 	while (GetMessage (&msg, NULL, 0, 0))
 	{
  		if (!TranslateAccelerator (hwnd_frame, hAccel, &msg))
		{
			TranslateMessage (&msg) ;
            
            /* �����Ҫ�Ļ�����������޸ĸ�����Ϣ�Ĵ���
            if (msg.message==WM_KEYDOWN && VK_RETURN==msg.wParam && msg.hwnd==hwnd_origin)
            {
                msg.hwnd = hwnd_new;
            }
            */
            
			DispatchMessage (&msg) ;
  		}
 	}

    return msg.wParam ;
}


