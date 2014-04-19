/* 
 * �����Ϊ��ѡ���Դ�����
 * ������İ�Ȩ(����Դ�뼰�����Ʒ����汾)��һ�й������С�
 * ����������ʹ�á������������
 * ��Ҳ�������κ���ʽ���κ�Ŀ��ʹ�ñ����(����Դ�뼰�����Ʒ����汾)���������κΰ�Ȩ���ơ�
 * =====================
 * ����: ������
 * ����: sunmingbao@126.com
 */

#ifndef __COMMON_H_
#define __COMMON_H_

#include <tchar.h> 
#include <commctrl.h>
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>

#include "defs.h"
#include "app_property.h"


void add_tip(HWND htip_control, HWND hwnd, TCHAR *tips);
static inline void refresh_window(HWND hwnd)
{
    InvalidateRect(hwnd, NULL, TRUE);
    UpdateWindow(hwnd);
}

static inline HANDLE launch_thread(LPTHREAD_START_ROUTINE ThreadFunc, LPVOID lpParameter)
{
    HANDLE hThrd;
    DWORD threadId;
    
    hThrd = CreateThread(NULL,
        0,
        ThreadFunc,
        lpParameter,
        0,
        &threadId );
    return hThrd;

}
void play_sound_async(LPCTSTR pszSound,  DWORD fdwSound);
void enable_sound();
void disable_sound();
int  is_sound_enabled();
int rand_time_range(int min, int max);
void set_int_text(HWND hwnd, int value);
#endif

