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
#include <stdlib.h>
#include "global_symbols.h"
#include "app_property.h"
#include "res.h"

#include "common.h"
#include "utils.h"


int doc_modified;

char doc_data[64];

void save_doc_file(TCHAR *file_name)
{
    FILE *file=fopen(file_name, "wb");
    fwrite(doc_data, sizeof(doc_data), 1, file);
    fclose(file);
}

void load_doc_file(TCHAR *file_name)
{
    FILE *file=fopen(file_name, "rb");
    fread(doc_data, sizeof(doc_data), 1, file);
    fclose(file);
}

