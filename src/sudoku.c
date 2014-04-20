/* 
 * 本软件为免费、开源软件。
 * 本软件的版权(包括源码及二进制发布版本)归一切公众所有。
 * 您可以自由使用、传播本软件。
 * 您也可以以任何形式、任何目的使用本软件(包括源码及二进制发布版本)，而不受任何版权限制。
 * =====================
 * 作者: 孙明保
 * 邮箱: sunmingbao@126.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include "sudoku.h"

const char empty_stage_str[] =
"........."
"........."
"........."
"........."
"........."
"........."
"........."
"........."
".........";

void add_pos(t_pos_set *pt_pos_set, t_pos t_pos)
{
    assert(pt_pos_set->num<MAX_POS_NUM);

    pt_pos_set->at_pos[pt_pos_set->num] = t_pos;
    pt_pos_set->num++;
}

int pos_set_has_pos(t_pos_set *pt_pos_set, t_pos *pt_pos)
{
    int i;
    for (i=0; i<pt_pos_set->num; i++)
    {
        if (pos_equal(&(pt_pos_set->at_pos[i]), pt_pos))
            return 1;

    }

    return 0;
}

void grid_del_candi(t_board *pt_board, int row, int col, char candi)
{
    int i;
    t_grid *pt_grid = &(pt_board->at_grid[row][col]);
    if (UNDER_DOING != pt_board->state)  return;
    if (grid_has_value(pt_board, row, col)) return;
    if (!grid_has_candi(pt_board, row, col, candi)) return;
    pt_grid->ac_candi[candi-MIN_DIGIT_CHAR] = NO_VALUE_CHAR;
    pt_grid->candi_num--;
    if (0==pt_grid->candi_num)
    {
        pt_board->state = NO_SOLUTION;
    }
#if 0
    if (1==pt_grid->candi_num)
    {
        for (i = 0; i<MAX_DIGIT_NUM; i++)
        {
            if (pt_grid->ac_candi[i] != NO_VALUE_CHAR)
            {
                assign_grid(pt_board, row, col, pt_grid->ac_candi[i]);
                return;
            }
        }
    }
#endif
}

void grid_set_value(t_board *pt_board, int row, int col, char value)
{
    t_grid *pt_grid = &(pt_board->at_grid[row][col]);
    pt_grid->value = value;
    pt_board->nr_grids_left--;
    if (0==pt_board->nr_grids_left)
        pt_board->state = SUCC_DONE;
}

void init_grid(t_grid *pt_grid)
{
    int i;
    pt_grid->value = NO_VALUE_CHAR;
    pt_grid->candi_num = MAX_DIGIT_NUM;
    for (i = 0; i < MAX_DIGIT_NUM; i++)
        pt_grid->ac_candi[i] = '1' + i;

    pt_grid->ac_candi[MAX_DIGIT_NUM] = '\0';
}

void init_board(t_board *pt_board)
{
    int i, j;
    pt_board->state = UNDER_DOING;
    pt_board->nr_grids_left = MAX_GRID_NUM;
    for (i = 0; i < MAX_ROW_NUM; i++)
    {
        for (j = 0; j < MAX_COL_NUM; j++)
        {
            init_grid(&(pt_board->at_grid[i][j]));

        }
    }
}

void get_row(t_pos_set *pt_pos_set, int row, int col)
{
    int i;
    for (i = 0; i<MAX_COL_NUM; i++)
    {
        add_pos(pt_pos_set, make_pos(row, i));
    }
}

void get_col(t_pos_set *pt_pos_set, int row, int col)
{
    int i;
    for (i = 0; i<MAX_ROW_NUM; i++)
    {
        add_pos(pt_pos_set, make_pos(i, col));
    }
}

void get_blk(t_pos_set *pt_pos_set, int row, int col)
{
    int i, j;
    row -= (row % 3);
    col -= (col % 3);
    for (i = 0; i<3; i++)
    {
        for (j = 0; j<3; j++)
        {
            add_pos(pt_pos_set, make_pos(row+i, col+j));
        }
    }
}

void get_nbs(t_pos_set *pt_pos_set, int row, int col)
{
    pt_pos_set->num = 0;
    get_row(pt_pos_set, row, col);
    get_col(pt_pos_set, row, col);
    get_blk(pt_pos_set, row, col);
}

t_pos get_grid_pos_with_min_candi(t_board *pt_board)
{
    int min_candi_num = 10;
    t_pos  pos;
    int i, j;
    t_grid *pt_grid;
    
    for (i = 0; i < MAX_ROW_NUM; i++)
    {
        for (j = 0; j < MAX_COL_NUM; j++)
        {
            if (grid_has_value(pt_board, i, j))
                continue;
            
            pt_grid = &(pt_board->at_grid[i][j]);
            if (pt_grid->candi_num<min_candi_num)
            {
                min_candi_num = pt_grid->candi_num;
                pos = make_pos(i, j);
            }

        }
    }

    return pos;
}

t_value_set get_grid_candis(t_board *pt_board, t_pos pos)
{
    t_value_set  values = {0};
    t_grid *pt_grid = &(pt_board->at_grid[pos.row][pos.col]);
    int i;

    for (i = 0; i < MAX_DIGIT_NUM; i++)
    {
        if (pt_grid->ac_candi[i] != NO_VALUE_CHAR)
        {
            values.value[values.num] = pt_grid->ac_candi[i];
            values.num++;
        }
    }

    return values;
}

void broad_cast_constrain(t_board *pt_board, int row, int col, char value)
{
    t_pos_set pos_set;
    int i;
    get_nbs(&pos_set, row, col);
    for (i=0; i<pos_set.num; i++)
    {
        grid_del_candi(pt_board
            , pos_set.at_pos[i].row
            , pos_set.at_pos[i].col
            , value);
    }
}

int nbs_have_value(t_board *pt_board, int row, int col, char value)
{
    t_pos_set pos_set;
    int i;
    get_nbs(&pos_set, row, col);
    for (i=0; i<pos_set.num; i++)
    {
        if (!grid_has_value(pt_board, pos_set.at_pos[i].row, pos_set.at_pos[i].col)) continue;
        if (grid_value(pt_board, pos_set.at_pos[i].row, pos_set.at_pos[i].col)==value)
        {
            pt_board->tmp_pos.row=pos_set.at_pos[i].row;
            pt_board->tmp_pos.col=pos_set.at_pos[i].col;
            return 1;
        }
    }

    return 0;
}

int nbs_have_only_this_candi(t_board *pt_board, int row, int col, char value)
{
    t_pos_set pos_set;
    int i;
    t_grid *pt_grid;
    get_nbs(&pos_set, row, col);
    for (i=0; i<pos_set.num; i++)
    {
        if (pos_set.at_pos[i].row==row && pos_set.at_pos[i].col==col) continue;
        pt_grid = &(pt_board->at_grid[pos_set.at_pos[i].row][pos_set.at_pos[i].col]);
        if (grid_has_value(pt_board, pos_set.at_pos[i].row, pos_set.at_pos[i].col)) continue;
        if (pt_grid->candi_num!=1) continue;
        if (grid_has_candi(pt_board, pos_set.at_pos[i].row, pos_set.at_pos[i].col, value))
        {
            pt_board->tmp_pos.row=pos_set.at_pos[i].row;
            pt_board->tmp_pos.col=pos_set.at_pos[i].col;
            return 1;
        }
    }

    return 0;
}

void assign_grid(t_board *pt_board, int row, int col, char value)
{
    //printf("%d %d %c\n", row, col, value);
    //t_grid *pt_grid = &(pt_board->at_grid[row][col]);
    if (UNDER_DOING != pt_board->state)  return;
    if (grid_has_value(pt_board, row, col)) return;
    if (!grid_has_candi(pt_board, row, col, value))
    {
        pt_board->state = NO_SOLUTION;
        return;
    }
    grid_set_value(pt_board, row, col, value);
    broad_cast_constrain(pt_board, row, col, value);

}

int format_input(char *output, char *input)
{
    int cnt = 0;
    char *pchar = input;



        while ((*pchar)!=0)
        {
            if ((*pchar) == '.' || ((*pchar) >= '1' && (*pchar) <= '9'))
            {
                output[cnt] = *pchar;
                cnt++;

                if (cnt>=81) break;
            }
            pchar++;
        }


    output[cnt] = 0;

    return cnt>=81;

}

void load_game(t_board *pt_board, const char *input)
{
    int i,j;
    int row, col;
    init_board(pt_board);
    for (i=0;i<MAX_GRID_NUM; i++)
    {
        row = i/MAX_COL_NUM;
        col = i%MAX_COL_NUM;

        if (isdigit(input[i]))
        assign_grid(pt_board, row, col, input[i]);
    }
}

void board_to_input_str(char *buf, t_board *ptBoard)
{
    int i=0;
    int row, col;

    for (row=0; row<MAX_ROW_NUM; row++)
    {
        for (col=0; col<MAX_COL_NUM; col++)
        {
            if (grid_has_value(ptBoard, row, col))
                buf[i] = grid_value(ptBoard, row, col);
            else
                buf[i] = NO_VALUE_CHAR;

            i++;

        }
    }
   
    buf[i] = 0;

}


int need_stop;

void stop_resolve()
{
    need_stop = 1;
}

void grid_del_candi_smart(t_board *pt_board, int row, int col, char candi)
{
    int i;
    t_grid *pt_grid = &(pt_board->at_grid[row][col]);
    if (UNDER_DOING != pt_board->state)  return;
    if (grid_has_value(pt_board, row, col)) return;
    if (!grid_has_candi(pt_board, row, col, candi)) return;
    pt_grid->ac_candi[candi-MIN_DIGIT_CHAR] = NO_VALUE_CHAR;
    pt_grid->candi_num--;
    if (0==pt_grid->candi_num)
    {
        pt_board->state = NO_SOLUTION;
    }
#if 1
    if (1==pt_grid->candi_num)
    {
        for (i = 0; i<MAX_DIGIT_NUM; i++)
        {
            if (pt_grid->ac_candi[i] != NO_VALUE_CHAR)
            {
                assign_grid_smart(pt_board, row, col, pt_grid->ac_candi[i]);
                return;
            }
        }
    }
#endif
}

void broad_cast_constrain_smart(t_board *pt_board, int row, int col, char value)
{
    t_pos_set pos_set;
    int i;
    get_nbs(&pos_set, row, col);
    for (i=0; i<pos_set.num; i++)
    {
        grid_del_candi_smart(pt_board
            , pos_set.at_pos[i].row
            , pos_set.at_pos[i].col
            , value);
    }
}

void assign_grid_smart(t_board *pt_board, int row, int col, char value)
{
    //printf("%d %d %c\n", row, col, value);
    //t_grid *pt_grid = &(pt_board->at_grid[row][col]);
    if (UNDER_DOING != pt_board->state)  return;
    if (grid_has_value(pt_board, row, col)) return;
    if (!grid_has_candi(pt_board, row, col, value))
    {
        pt_board->state = NO_SOLUTION;
        return;
    }
    grid_set_value(pt_board, row, col, value);
    broad_cast_constrain_smart(pt_board, row, col, value);

}

void enhanced_solve(t_board *ptBoard, uint64_t max_return_num, uint64_t  *presult_num, callback_for_each_result callback)
{
    t_pos pos;
    t_value_set values;
    int i;
    t_board board;
//DbgPrintf("enhanced_solve");
    pos = get_grid_pos_with_min_candi(ptBoard);
    values = get_grid_candis(ptBoard, pos);

    for (i=0; i<values.num; i++)
    {
        if (max_return_num && ((*presult_num) >= max_return_num))
        return;

        if (need_stop) return;

        board = *ptBoard;
        assign_grid(&board, pos.row, pos.col, values.value[i]);

        if (board_solved(&board))
        {
            (*presult_num)++;
            if (callback!=NULL)
            {
                callback(*presult_num, &board);
            }

            continue;

        }
        else if (board_no_solution(&board))
        {
            continue;
        }
        else
            enhanced_solve(&board, max_return_num, presult_num, callback);

    }
}

uint64_t solve(const char *input, uint64_t max_return_num, callback_for_each_result callback)
{
    uint64_t  result_num = 0;
    t_board board;

    need_stop = 0;
    load_game(&board, input);

    if (board_solved(&board))
    {
        result_num++;
        if (callback!=NULL)
        {
            callback(result_num, &board);
        }
        return result_num;

    }

    enhanced_solve(&board, max_return_num, &result_num, callback);
    return result_num;
}

#if 0
int main()
{
    printf("Hello world!\n");
    solve(" 57 6   3"
          "68  94  5"
          " 13    2 "
          " 76  3  2"
          "  85 13  "
          "1  6  57 "
          " 2    86 "
          "3  81  57"
          "8   4 93 ");
    return 0;
}
#endif


