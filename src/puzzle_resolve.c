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

    sprintf(buf, "第 %d 个解\n", (int)result_idx);
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
    sprintf(buf, "全部解的个数: %-22d\n\n", 0);
    fputs(buf, fp_solve_result);

    solve(input, 0, for_each_result);

    sprintf(buf, "全部解的个数: %-22d", (int)result_num);
    rewind(fp_solve_result);
    fputs(buf, fp_solve_result);

    fclose(fp_solve_result);

    ShellExecute(NULL, "open"
                    , "result.txt"
                    , NULL, NULL, SW_SHOWNORMAL);

}

