/* 
 * �����Ϊ��ѡ���Դ�����
 * ������İ�Ȩ(����Դ�뼰�����Ʒ����汾)��һ�й������С�
 * ����������ʹ�á������������
 * ��Ҳ�������κ���ʽ���κ�Ŀ��ʹ�ñ����(����Դ�뼰�����Ʒ����汾)���������κΰ�Ȩ���ơ�
 * =====================
 * ����: ������
 * ����: sunmingbao@126.com
 */

#ifndef __FILE_OPEN_HISTORY_H_
#define __FILE_OPEN_HISTORY_H_
#include <windows.h>
#include <tchar.h> 
#include <windowsx.h>

int get_history_arch_file_by_idx(int idx, char *field_value);
void update_arch_file_history(char *file_name);
void populate_recent_arch_files(HMENU hMenu);
int get_history_sdpzl_file_by_idx(int idx, char *field_value);
void update_sdpzl_file_history(char *file_name);
void populate_recent_sdpzl_files(HMENU hMenu);
#endif

