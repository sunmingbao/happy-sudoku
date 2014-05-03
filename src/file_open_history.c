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

#include "res.h"


#define    HISTORY_FILE_NAME    "history"
#define    HISTORY_FILE_TMP    "history.tmp"
#define    HISTORY_FILE_NAME_SDPZL    ".\\history_sdpzl"

int read_next_filed(FILE *the_file, char *field_name, char *field_value)
{
    char line[MAX_FILE_PATH_LEN + 32];
    char *sep, *tail;

    if (NULL==fgets(line, sizeof(line), the_file)) 
        return 1;

    if (strlen(line)<3) return 1;

    sep = strchr(line,'=');
    if (sep==NULL) return 1;

    *sep = 0;

    tail = sep + strlen(sep+1);
    while ((*tail == '\r') || (*tail == '\n')|| (*tail == ' '))
    {
        *tail=0;
        tail--;
    }
    
    if (field_name!=NULL)
        strcpy(field_name, line);

    if (field_value!=NULL)
        strcpy(field_value, sep+1);

    return 0;
}

int get_field_value_by_idx(char *file_path
                        ,int idx
                        ,char *field_value)
{
    FILE *history_file = fopen(file_path, "r");
    int i;
    int ret;
    
    if (history_file==NULL) return 1;
    
    for(i=0; i<=idx; i++)
    {
        ret=read_next_filed(history_file, NULL, field_value);
    }

    fclose(history_file);
    return ret;
}

void populate_recent_files(HMENU hMenu, int first_menu_item_id, char *history_file_name)
{
    int i;
    int file_num;
    char field_value[MAX_FILE_PATH_LEN];
    char    menu_name[MAX_FILE_PATH_LEN + 8];
    FILE *history_file;
    

    for (i=0; i<MAX_RECENT_FILE_NUM; i++)
        DeleteMenu(hMenu, 0, MF_BYPOSITION);


    if (!file_exists(history_file_name))
    {
        //fclose(fopen(HISTORY_FILE_NAME, "w"));
        return;
    }

    history_file = fopen(history_file_name, "r");
    read_next_filed(history_file, NULL, field_value);
    file_num = atoi(field_value);

    for (i=0; i<file_num; i++)
    {
        read_next_filed(history_file, NULL, field_value);
        sprintf(menu_name, "&%d  %s", i, field_value);
        AppendMenu(hMenu, MF_STRING, first_menu_item_id+i,  menu_name) ;
    }

    fclose(history_file);
}

void update_file_history(char *file_name, char *history_file_name)
{
    FILE *history_file_tmp = NULL;
    FILE *history_file = NULL;
    char field_value[MAX_FILE_PATH_LEN];
    char line[MAX_FILE_PATH_LEN+64];
    int len, i;
    int old_file_num, new_file_num = 1;

    history_file_tmp = fopen(HISTORY_FILE_TMP, "w");
    len=sprintf(field_value
        , "file_num=1  \n"
          "file_0=%s\n"
          , file_name);
    fwrite(field_value, 1, len, history_file_tmp);

    if (!file_exists(history_file_name))
    {
        goto EXIT;
    }

    history_file = fopen(history_file_name, "r");
    read_next_filed(history_file, NULL, field_value);
    old_file_num = atoi(field_value);

    for (i=0; i<old_file_num; i++)
    {

        read_next_filed(history_file, NULL, field_value);
        if (0==strcmp(field_value, file_name)) continue;
        len=sprintf(line, "file_%d=%s\n", new_file_num, field_value);
        fwrite(line, 1, len, history_file_tmp);
        new_file_num++;
        if (new_file_num>=MAX_RECENT_FILE_NUM) break;

    }

    fseek(history_file_tmp, 0, SEEK_SET);
    len=sprintf(line, "file_num=%d", new_file_num);
    fwrite(line, 1, len, history_file_tmp);

EXIT:
    fclose(history_file_tmp);
    if (history_file != NULL)
    {
        fclose(history_file);
        delete_file_f(history_file_name);
    }
    MoveFile(HISTORY_FILE_TMP, history_file_name);

    
}

int get_history_arch_file_by_idx(int idx, char *field_value)
{
    return get_field_value_by_idx(HISTORY_FILE_NAME, idx, field_value);
}

void update_arch_file_history(char *file_name)
{
    update_file_history(file_name, HISTORY_FILE_NAME);
}

void populate_recent_arch_files(HMENU hMenu)
{
    populate_recent_files(hMenu, ID_FILE_RECENT_FILE_BEGIN, HISTORY_FILE_NAME);
}


int get_history_sdpzl_file_by_idx(int idx, char *field_value)
{
    return get_field_value_by_idx(HISTORY_FILE_NAME_SDPZL, idx, field_value);
}

void update_sdpzl_file_history(char *file_name)
{
    update_file_history(file_name, HISTORY_FILE_NAME_SDPZL);
}

void populate_recent_sdpzl_files(HMENU hMenu)
{
    populate_recent_files(hMenu, ID_RECENT_PUZZLE_FILE_BEGIN, HISTORY_FILE_NAME_SDPZL);
}

