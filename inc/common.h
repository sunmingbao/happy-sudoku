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

#endif

