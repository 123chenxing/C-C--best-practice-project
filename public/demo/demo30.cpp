/*
 *  ��������demo30.cpp���˳�����ʾ��������в���MKDIR�������ݾ���·�����ļ�����Ŀ¼���𼶵Ĵ���Ŀ¼��
 *  ���ߣ������
*/
#include "../_public.h"

int main()
{
  MKDIR("/tmp/aaa/bbb/ccc/ddd",false);   // ����"/tmp/aaa/bbb/ccc/ddd"Ŀ¼��

  MKDIR("/tmp/111/222/333/444/data.xml",true);   // ����"/tmp/111/222/333/444"Ŀ¼��
}

