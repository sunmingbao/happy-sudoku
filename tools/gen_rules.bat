:: 
:: �����Ϊ��ѡ���Դ�����
:: ������İ�Ȩ(����Դ�뼰�����Ʒ����汾)��һ�й������С�
:: ����������ʹ�á������������
:: ��Ҳ�������κ���ʽ���κ�Ŀ��ʹ�ñ����(����Դ�뼰�����Ʒ����汾)���������κΰ�Ȩ���ơ�
:: =====================
:: ����: ������
:: ����: sunmingbao@126.com
::


:: ����makefile����

@echo off
set rule_file=obj\rules.txt

echo #gen_rules > %rule_file%
for /f  "delims=  " %%i in (obj\src_2.txt) do (
echo %%i
echo #rule begin >>%rule_file%
>>%rule_file% set/p=${OBJ_DIR}\<nul
>>%rule_file% set/p=%%i<nul
>>%rule_file% set/p=.o : %%i<nul
echo .c>>%rule_file%
type tools\compile_cmd.txt >>%rule_file%
)


echo %rule_file%

