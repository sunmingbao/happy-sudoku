/* 
 * 本软件为免费、开源软件。
 * 本软件的版权(包括源码及二进制发布版本)归一切公众所有。
 * 您可以自由使用、传播本软件。
 * 您也可以以任何形式、任何目的使用本软件(包括源码及二进制发布版本)，而不受任何版权限制。
 * =====================
 * 作者: 孙明保
 * 邮箱: sunmingbao@126.com
 */

#ifndef __SUDOKU_H_
#define __SUDOKU_H_

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define    MAX_ROW_NUM    (9)
#define    MAX_COL_NUM    (9)
#define    MAX_GRID_NUM   (81)
#define    MAX_DIGIT_NUM  (9)
#define    MAX_POS_NUM    (27)

#define    MIN_DIGIT_CHAR  ('1')
#define    NO_VALUE_CHAR  ('.')
typedef struct
{
    int row;
    int col;
} t_pos;

typedef struct
{
    int num;
    t_pos at_pos[MAX_POS_NUM];
} t_pos_set;



t_pos static inline make_pos(int row, int col)
{
    t_pos tmp_pos;
    tmp_pos.row = row;
    tmp_pos.col = col;
    return tmp_pos;
}


typedef struct
{
    char value;
    int  candi_num;
    char ac_candi[MAX_DIGIT_NUM+1];
} t_grid;

typedef struct
{
    int state;
    int nr_grids_left;
    t_pos tmp_pos;
    t_grid  at_grid[MAX_ROW_NUM][MAX_COL_NUM];
} t_board;

typedef enum
{
    UNDER_DOING = 1,
    NO_SOLUTION,
    SUCC_DONE
} E_BOARD_STATE;

void load_game(t_board *pt_board, const char *input);
static inline char grid_value(t_board *pt_board, int row, int col)
{
    t_grid *pt_grid = &(pt_board->at_grid[row][col]);
    return (pt_grid->value);
}

static inline int grid_has_value(t_board *pt_board, int row, int col)
{
    t_grid *pt_grid = &(pt_board->at_grid[row][col]);
    return (pt_grid->value != NO_VALUE_CHAR);
}

static inline int grid_has_candi(t_board *pt_board, int row, int col, char candi)
{
    t_grid *pt_grid = &(pt_board->at_grid[row][col]);
    return (pt_grid->ac_candi[candi-MIN_DIGIT_CHAR] != NO_VALUE_CHAR);
}

void assign_grid(t_board *pt_board, int row, int col, char value);
void get_nbs(t_pos_set *pt_pos_set, int row, int col);
int nbs_have_only_this_candi(t_board *pt_board, int row, int col, char value);
int nbs_have_value(t_board *pt_board, int row, int col, char value);
static inline int board_solved(t_board *pt_board)
{
    return SUCC_DONE==pt_board->state;
}

static inline int board_no_solution(t_board *pt_board)
{
    return NO_SOLUTION==pt_board->state;
}


#endif

