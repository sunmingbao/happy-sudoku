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
#include <stdio.h>
#include "gui.h"


int scrn_width;
int scrn_height;

int cxChar, cyChar;
int cxChar_2, cyChar_2;
int cxChar_20, cyChar_20;
t_gui_size_scale_ref gt_gui_size_scale_ref = 
{
    .scrn_width  = 1440,
    .scrn_height = 900,
    .cxChar      = 10,
    .cyChar      = 20,
};
void get_sys_gui_info()
{
    scrn_width = GetSystemMetrics(SM_CXSCREEN);
    scrn_height = GetSystemMetrics(SM_CYSCREEN);
}

HFONT  char_font, char_font_2, char_font_25, fixedsys_font;
static HFONT  create_font(int height, TCHAR *font_name, int use_black)
{
    HFONT  h_font;
    LOGFONT lf ;
    
    memset(&lf, 0, sizeof(lf));
    lf.lfCharSet = 1;

    /* font_name�����Fixedsys��height�������� */
    lf.lfHeight = height;
    
    lf.lfPitchAndFamily = FIXED_PITCH;

    if (use_black)
        lf.lfWeight  = FW_BLACK;

    if (NULL != font_name)
        strcpy(lf.lfFaceName, font_name);
    
    h_font = CreateFontIndirect(&lf);

    return h_font;
}

void get_font_size(HFONT  the_font, int *width, int *height)
{
    HDC hdc;
    TEXTMETRIC textmetric;

    hdc = CreateIC (TEXT ("DISPLAY"), NULL, NULL, NULL) ;
    SelectObject(hdc, the_font) ;
    GetTextMetrics(hdc, &textmetric) ;
    SelectObject (hdc, GetStockObject(SYSTEM_FONT)) ;
    DeleteDC (hdc) ;
    
    *width = textmetric.tmAveCharWidth ;
    *height = textmetric.tmHeight + textmetric.tmExternalLeading ;

}

void init_gui_info()
{
    get_sys_gui_info();
    if (scrn_width==1440 && scrn_height==900)
        char_font = create_font(0, "Fixedsys", 0);
    else
        char_font = create_font(gt_gui_size_scale_ref.cyChar*WIDTH_COEFFICIENT, "Courier New", 1);
    char_font_2 = create_font(gt_gui_size_scale_ref.cyChar*7/6*WIDTH_COEFFICIENT, TEXT("����"), 1);
    char_font_25 = create_font(25, TEXT("Courier New"), 0);
   fixedsys_font = create_font(0, "Fixedsys", 0);
    get_font_size(char_font, &cxChar, &cyChar);
    get_font_size(char_font_2, &cxChar_2, &cyChar_2);
}

