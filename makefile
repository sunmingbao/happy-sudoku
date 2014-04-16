# 
# �����Ϊ��ѡ���Դ�����
# ������İ�Ȩ(����Դ�뼰�����Ʒ����汾)��һ�й������С�
# ����������ʹ�á������������
# ��Ҳ�������κ���ʽ���κ�Ŀ��ʹ�ñ����(����Դ�뼰�����Ʒ����汾)���������κΰ�Ȩ���ơ�
# =====================
# ����: ������
# ����: sunmingbao@126.com
# 

BUILD_TIME:=$(shell echo %date:~0,10%) $(shell echo %time:~0,8%)

CC := gcc

CFLAG := -Wall -O2  -DBUILD_TIME='"$(BUILD_TIME)"' -D_WIN32_IE=0x0501 -D_WIN32_WINNT=0x0502 -DWINVER=0x0501 

# ���Ҫ����_UNICODE�汾����ſ������ע��
#CFLAG := $(CFLAG) -D_UNICODE

LDFLAG := -mwindows  -s  -lkernel32 -luser32 -lgdi32 -lcomctl32 -lws2_32 -lWinmm 


PRJ_DIR := .
RES_DIR := $(PRJ_DIR)\res
INC_RES_DIRS := --include-dir $(RES_DIR)
INC_DIRS := -I$(PRJ_DIR)\inc -I$(RES_DIR)
OBJ_DIR:=$(PRJ_DIR)\obj

PREPAIR_OBJ_DIR:=$(shell mkdir $(OBJ_DIR))
PREPAIR_OBJ_DIR:=$(shell del /F /Q $(OBJ_DIR)\\*)
PREPAIR_OBJ_DIR:=$(shell copy $(PRJ_DIR)\other_files\stages.txt $(OBJ_DIR)\)
PREPAIR_OBJ_DIR:=$(shell copy $(PRJ_DIR)\other_files\music.MID $(OBJ_DIR)\)
PREPAIR_OBJ_DIR:=$(shell copy $(PRJ_DIR)\other_files\music.mp3 $(OBJ_DIR)\)

SRC_FILES_BASE_NAMES:=$(shell tools\get_src_files.bat)
OBJ_FILES:=$(addprefix $(OBJ_DIR)\, $(addsuffix .o, $(SRC_FILES_BASE_NAMES)))

RES_OBJS := $(OBJ_DIR)\res.orc
ALL_OBJS := $(OBJ_FILES) $(RES_OBJS) 

CAUSE_GEN_RULES:=$(shell tools\gen_rules.bat)

VPATH := $(PRJ_DIR)\src

CFLAG := $(CFLAG) $(INC_DIRS)
WINDRES_FLAG := --use-temp-file $(INC_RES_DIRS)
# -------------------------------------------------------------------------
# Do not modify the rest of this file!
# -------------------------------------------------------------------------

### Targets: ###
target := $(OBJ_DIR)\happy_sudoku.exe

RES_OBJS := $(OBJ_DIR)\res.orc



default: $(target)


$(target): $(ALL_OBJS)
	$(CC) -o $@ $(ALL_OBJS)  $(LDFLAG)


$(OBJ_DIR)\res.orc: $(PRJ_DIR)\res\resource.rc
	windres $(WINDRES_FLAG) -i $< -o $@   


-include $(OBJ_DIR)\rules.txt

installer:
	makensis $(PRJ_DIR)\package.nsi
	
clean:
	-cmd.exe /c del /F /Q  $(OBJ_DIR)\\*

all: default installer

.PHONY: all default installer clean



