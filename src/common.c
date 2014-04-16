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
#include "common.h"
#include <tchar.h>     
#include <stdio.h>

#include "global_symbols.h"




void add_tip(HWND htip_control, HWND hwnd, TCHAR *tips)
{
    TOOLINFO        ti;

    ti.cbSize = sizeof(TOOLINFO);
    //Flags TTF_IDISHWND show that the uID is the handle to control
    ti.uFlags =  TTF_IDISHWND | TTF_SUBCLASS;  

    //Handle of the Control for which you want to Tooltip to be displayed
    ti.uId    =(UINT)hwnd;

    //Handle of the parent window (The window in which the Control resides)
    ti.hwnd   = hwnd;
    ti.hinst  = g_hInstance;
    ti.lpszText  = tips;         //Text you want as a Tooltip
    	//Below is the Rectangle for ToolTip. But It will be ignored 
    	//If you use TTF_IDISHWND in Flag
    //ti.rect.left = ti.rect.top = ti.rect.bottom = ti.rect.right = 0; 

    //Will Activate the tooltip window
    SendMessage(htip_control,TTM_ACTIVATE,TRUE,0);  

    //will Add the Tooltip Window
    SendMessage(htip_control,TTM_ADDTOOL,0,(LPARAM)&ti);
}


TCHAR para_pszSound[64];
HMODULE para_hmod;
DWORD   para_fdwSound;

int sound_enabled = 1;

DWORD WINAPI  play_sound_thread(LPVOID lpParameter)
{
    PlaySound(
    para_pszSound, 
    para_hmod,  
    para_fdwSound);

    return 0;
}


void play_sound_async(LPCTSTR pszSound,  DWORD fdwSound)
{
    if (!is_sound_enabled()) return;
    if (NULL==para_hmod) para_hmod=GetModuleHandle(NULL);
    strcpy(para_pszSound, pszSound);
    para_fdwSound = fdwSound;

    launch_thread(play_sound_thread, NULL);
}

void enable_sound()
{
    sound_enabled = 1;
}

void disable_sound()
{
    sound_enabled = 0;
}

int is_sound_enabled()
{
    return sound_enabled;
}


