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

int get_last_doc_file(char *file_path);
void populate_recent_files(HMENU	 hMenu);
void update_file_open_history(char *file_path);
int get_file_path_by_idx(char *file_path, int idx);

#endif

