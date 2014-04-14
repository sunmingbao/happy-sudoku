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

#include "ext_utils_textout.h"

void fw_text_out_full(HDC hdc, int x, int y, int height
    ,COLORREF bg, COLORREF fg, TCHAR *text, int len)
{
    HFONT  h_old_font;
    LOGFONT lf ;
    
    memset(&lf, 0, sizeof(lf));
    lf.lfCharSet = 1;

    lf.lfHeight = height;
    lf.lfPitchAndFamily = FIXED_PITCH;
    lf.lfWeight  = FW_BLACK;

    
    h_old_font = SelectObject (hdc, CreateFontIndirect(&lf)) ;

    SetBkMode(hdc, OPAQUE);
    SetBkColor(hdc, bg) ;
    SetTextColor(hdc, fg) ;
    TextOut(hdc, x, y, text, len) ; 

    DeleteObject(SelectObject(hdc, h_old_font)) ;
}

void fw_text_out_full_trans(HDC hdc, int x, int y, int height
    ,COLORREF fg, TCHAR *text, int len)
{
    HFONT  h_old_font;
    LOGFONT lf ;
    
    memset(&lf, 0, sizeof(lf));
    lf.lfCharSet = 1;

    lf.lfHeight = height;
    lf.lfPitchAndFamily = FIXED_PITCH;
    lf.lfWeight  = FW_BLACK;

    
    h_old_font = SelectObject(hdc, CreateFontIndirect(&lf)) ;

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, fg) ;
    TextOut(hdc, x, y, text, len) ; 

    DeleteObject(SelectObject(hdc, h_old_font)) ;
}

int fw_text_get_char_width(int height)
{
    HFONT  h_old_font;
    LOGFONT lf ;
    HDC hdc = CreateIC(TEXT ("DISPLAY"), NULL, NULL, NULL) ;
    TEXTMETRIC textmetric;
    
    memset(&lf, 0, sizeof(lf));
    lf.lfCharSet = 1;

    lf.lfHeight = height;
    lf.lfPitchAndFamily = FIXED_PITCH;
    lf.lfWeight  = FW_BLACK;


    
    h_old_font = SelectObject (hdc, CreateFontIndirect(&lf)) ;
    GetTextMetrics(hdc, &textmetric) ;
    DeleteObject(SelectObject(hdc, h_old_font)) ;
    DeleteDC(hdc) ;

    return textmetric.tmAveCharWidth ;

}

void fw_text_out_middle_trans(HDC hdc, RECT *rect, int height
    ,COLORREF fg, TCHAR *text, int len)
{
    HFONT  h_old_font;
    LOGFONT lf ;
    
    memset(&lf, 0, sizeof(lf));
    lf.lfCharSet = 1;

    lf.lfHeight = height;
    lf.lfPitchAndFamily = FIXED_PITCH;
    lf.lfWeight  = FW_BLACK;

    
    h_old_font = SelectObject(hdc, CreateFontIndirect(&lf)) ;

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, fg) ;
    DrawText (hdc, text, len, rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER) ; 

    DeleteObject(SelectObject(hdc, h_old_font)) ;
}

