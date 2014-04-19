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

void generate_digit_sequence(char *output)
{
    strcpy(output, "123456987");
}


t_pos_set generate_pos_sequence()
{
    t_pos_set  pos_set = {0};

    t_pos  pos;

    while (pos_set.num < 9)
    {
        pos = make_pos(rand_time_range(0, 8), rand_time_range(0, 8));
        if (pos_set_has_pos(&pos_set, &pos))
            continue;
        
        add_pos(&pos_set, pos);
       
    }

    return pos_set;
}


void generate_init_input(char *output)
{
    int i,j;
    int row, col;
    t_board board;
    char buf[10];
    t_pos_set  pos_set;
    
    init_board(&board);

    generate_digit_sequence(buf);
    pos_set = generate_pos_sequence();
    
    for (i=0;i<pos_set.num; i++)
    {
        assign_grid(&board
            , pos_set.at_pos[i].row
            , pos_set.at_pos[i].col
            , buf[i]);
    }

    board_to_input_str(output, &board);
}

char generated_input[128];
static void for_each_result(uint64_t result_idx, t_board *ptBoard)
{
    board_to_input_str(generated_input, ptBoard);
}

void random_del_some_inputs()
{
    int del_num = rand_time_range(30, 45);
    int idx;

    while (del_num)
    {
        idx = rand_time_range(0, 80);
        if (isdigit(generated_input[idx]))
        {
            generated_input[idx] = '.';
            del_num--;
        }
    }
    
}

void generate_puzzle(char *output)
{
    uint64_t result_num = 0;
    char buf[128];

    while (0==result_num)
    {
        generate_init_input(buf);
        result_num = solve(buf, 1, for_each_result);
    }

    random_del_some_inputs();
    strcpy(output, generated_input);
}

