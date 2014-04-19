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
#include "sudoku.h"
#include "res.h"

FILE *fp_solve_result;
uint64_t result_num;
static void for_each_result(uint64_t result_idx, t_board *ptBoard)
{
    char buf[128];
    char buf_2[128]={0};
    int i;

    result_num++;

    sprintf(buf, "�� %d ����\n", (int)result_idx);
    fputs(buf, fp_solve_result);

    board_to_input_str(buf, ptBoard);

    for (i=0; i<MAX_ROW_NUM; i++)
    {
        memcpy(buf_2, buf+i*9, 9);
        fputs(buf_2, fp_solve_result);
        fputs("\n", fp_solve_result);
    }

    fputs("\n", fp_solve_result);
}

void resolve_puzzle(const char *input)
{
    char buf[512];
    fp_solve_result = fopen("result.txt", "w");
    sprintf(buf, "ȫ����ĸ���: %-22d\n\n", 0);
    fputs(buf, fp_solve_result);

    solve(input, 0, for_each_result);

    sprintf(buf, "ȫ����ĸ���: %-22d", (int)result_num);
    rewind(fp_solve_result);
    fputs(buf, fp_solve_result);

    fclose(fp_solve_result);

    ShellExecute(NULL, "open"
                    , "result.txt"
                    , NULL, NULL, SW_SHOWNORMAL);

}

